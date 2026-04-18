// bpim_live.cpp
// BPIM live demo: /dev/video0 + face detection + marking
// Compile: g++ -O2 -std=c++17 bpim_live.cpp -o bpim_live `pkg-config --cflags --libs opencv4`

#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <vector>
#include <random>

using namespace cv;
using namespace std;

/* ---------- small utility macros ---------- */
#define IDX2(r,c,nc) ((r)*(nc) + (c))

/* ---------- linear algebra helpers (from your C code, adapted) ---------- */
static double dotp(const double *a, const double *b, int n) {
    double s=0.0; for(int i=0;i<n;++i) s += a[i]*b[i]; return s;
}
static double norm2(const double *a, int n) {
    double s=0.0; for(int i=0;i<n;++i) s += a[i]*a[i]; return sqrt(s);
}
static void scale_vec(double *a, int n, double alpha) { for(int i=0;i<n;++i) a[i]*=alpha; }
static void axpy(double alpha, const double *x, double *y, int n) { for(int i=0;i<n;++i) y[i] += alpha*x[i]; }

static void matmul(const double *A, const double *B, double *C, int rA, int cA, int cB) {
    for(int i=0;i<rA;++i) for(int j=0;j<cB;++j) {
        double s=0.0;
        for(int k=0;k<cA;++k) s += A[IDX2(i,k,cA)] * B[IDX2(k,j,cB)];
        C[IDX2(i,j,cB)] = s;
    }
}
static void matvec(const double *A, const double *x, double *y, int rows, int cols) {
    for(int i=0;i<rows;++i) {
        double s=0.0;
        for(int j=0;j<cols;++j) s += A[IDX2(i,j,cols)] * x[j];
        y[i] = s;
    }
}
static void transpose(const double *A, double *B, int r, int c) {
    for(int i=0;i<r;++i) for(int j=0;j<c;++j) B[IDX2(j,i,r)] = A[IDX2(i,j,c)];
}

/* ---------- Cholesky (lower triangular stored in A) ---------- */
static int cholesky(double *A, int n) {
    for(int i=0;i<n;++i) {
        for(int j=0;j<=i;++j) {
            double s = A[IDX2(i,j,n)];
            for(int k=0;k<j;++k) s -= A[IDX2(i,k,n)] * A[IDX2(j,k,n)];
            if (i==j) {
                if (s <= 1e-15) return -1;
                A[IDX2(i,j,n)] = sqrt(s);
            } else {
                A[IDX2(i,j,n)] = s / A[IDX2(j,j,n)];
            }
        }
        for(int j=i+1;j<n;++j) A[IDX2(i,j,n)] = 0.0;
    }
    return 0;
}
static void chol_solve(const double *L, const double *b, double *x, int n) {
    double *y = (double*)malloc(sizeof(double)*n);
    for(int i=0;i<n;++i) {
        double s = b[i];
        for(int k=0;k<i;++k) s -= L[IDX2(i,k,n)] * y[k];
        y[i] = s / L[IDX2(i,i,n)];
    }
    for(int i=n-1;i>=0;--i) {
        double s = y[i];
        for(int k=i+1;k<n;++k) s -= L[IDX2(k,i,n)] * x[k];
        x[i] = s / L[IDX2(i,i,n)];
    }
    free(y);
}

/* ---------- simplified Jacobi eigen (small K) ---------- */
static void jacobi_eigendecompose(double *A, double *eigvecs, double *eigvals, int n, int max_iter=1000, double tol=1e-10) {
    // init eigvecs = I
    for(int i=0;i<n;++i) for(int j=0;j<n;++j) eigvecs[IDX2(i,j,n)] = (i==j)?1.0:0.0;
    for(int it=0; it<max_iter; ++it) {
        int p=0,q=1; double max_off=0.0;
        for(int i=0;i<n;++i) for(int j=i+1;j<n;++j) {
            double v = fabs(A[IDX2(i,j,n)]);
            if (v > max_off) { max_off = v; p=i; q=j; }
        }
        if (max_off < tol) break;
        double app = A[IDX2(p,p,n)], aqq = A[IDX2(q,q,n)], apq = A[IDX2(p,q,n)];
        double phi = 0.5 * atan2(2.0*apq, (aqq - app));
        double c = cos(phi), s = sin(phi);
        // update A rows/cols
        for(int i=0;i<n;++i) {
            double aip = A[IDX2(i,p,n)], aiq = A[IDX2(i,q,n)];
            A[IDX2(i,p,n)] = c*aip - s*aiq;
            A[IDX2(i,q,n)] = s*aip + c*aiq;
        }
        for(int i=0;i<n;++i) {
            double api = A[IDX2(p,i,n)], aqi = A[IDX2(q,i,n)];
            A[IDX2(p,i,n)] = c*api - s*aqi;
            A[IDX2(q,i,n)] = s*api + c*aqi;
        }
        A[IDX2(p,q,n)] = 0.0; A[IDX2(q,p,n)] = 0.0;
        // eigvecs
        for(int i=0;i<n;++i) {
            double vip = eigvecs[IDX2(i,p,n)], viq = eigvecs[IDX2(i,q,n)];
            eigvecs[IDX2(i,p,n)] = c*vip - s*viq;
            eigvecs[IDX2(i,q,n)] = s*vip + c*viq;
        }
    }
    for(int i=0;i<n;++i) eigvals[i] = A[IDX2(i,i,n)];
}

/* ---------- BPIM model struct (minimal) ---------- */
struct BPIM {
    int N; int K; int k; int m;
    vector<double> U;   // K x N
    vector<double> mean; // N
    vector<double> phi;  // k x N
    vector<int> z;       // m
    vector<double> C;    // m x k
    vector<double> B;    // k x m
};

/* ---------- compute mean and center ---------- */
static void compute_mean_and_center(BPIM &M) {
    fill(M.mean.begin(), M.mean.end(), 0.0);
    for(int i=0;i<M.K;++i) for(int p=0;p<M.N;++p) M.mean[p] += M.U[IDX2(i,p,M.N)];
    for(int p=0;p<M.N;++p) M.mean[p] /= (double)M.K;
    for(int i=0;i<M.K;++i) for(int p=0;p<M.N;++p) M.U[IDX2(i,p,M.N)] -= M.mean[p];
}

/* ---------- compute eigenfaces (small-G trick) ---------- */
static int compute_eigenfaces(BPIM &M) {
    int K = M.K;
    vector<double> G(K*K, 0.0);
    double invK = 1.0 / (double)K;
    for(int i=0;i<K;++i) for(int j=0;j<K;++j) {
        double s=0.0;
        for(int p=0;p<M.N;++p) s += M.U[IDX2(i,p,M.N)] * M.U[IDX2(j,p,M.N)];
        G[IDX2(i,j,K)] = invK * s;
    }
    vector<double> eigvecsG(K*K,0.0), eigvalsG(K,0.0);
    vector<double> Gcopy = G;
    jacobi_eigendecompose(Gcopy.data(), eigvecsG.data(), eigvalsG.data(), K);

    // sort indices by descending eigenvalue
    vector<int> idx(K); for(int i=0;i<K;++i) idx[i]=i;
    for(int i=0;i<K-1;++i) {
        int max_i = i;
        for(int j=i+1;j<K;++j) if (eigvalsG[j] > eigvalsG[max_i]) max_i=j;
        if (max_i!=i) { swap(eigvalsG[i], eigvalsG[max_i]); swap(idx[i], idx[max_i]); }
    }

    M.phi.assign(M.k * M.N, 0.0);
    for(int i=0;i<M.k;++i) {
        int sel = idx[i];
        const double *v_i = &eigvecsG[IDX2(0, sel, K)];
        for(int p=0;p<M.N;++p) {
            double s=0.0;
            for(int l=0;l<K;++l) s += M.U[IDX2(l,p,M.N)] * v_i[l];
            M.phi[IDX2(i,p,M.N)] = s;
        }
        double nrm = norm2(&M.phi[IDX2(i,0,M.N)], M.N);
        if (nrm > 1e-15) scale_vec(&M.phi[IDX2(i,0,M.N)], M.N, 1.0/nrm);
    }
    return 0;
}

/* ---------- empirical interpolation points (greedy residual) ---------- */
static int empirical_interpolation_points(const vector<double> &phi, int k, int N, int m, vector<int> &z_out) {
    if (m < 1) return -1;
    int z1 = 0; double max_abs = 0.0;
    for(int p=0;p<N;++p) {
        double v = fabs(phi[IDX2(0,p,N)]);
        if (v > max_abs) { max_abs = v; z1 = p; }
    }
    z_out[0] = z1;
    for(int l=2; l<=m; ++l) {
        int idx_l = l-1;
        int prev = (idx_l < k) ? idx_l : k;
        vector<double> D(prev*prev, 0.0), d(prev,0.0);
        for(int j=0;j<prev;++j) {
            int zj = z_out[j];
            d[j] = phi[IDX2(idx_l, zj, N)];
            for(int i=0;i<prev;++i) D[IDX2(j,i,prev)] = phi[IDX2(i, zj, N)];
        }
        vector<double> Dt(prev*prev,0.0), DtD(prev*prev,0.0), Dt_d(prev,0.0);
        transpose(D.data(), Dt.data(), prev, prev);
        matmul(Dt.data(), D.data(), DtD.data(), prev, prev, prev);
        matvec(Dt.data(), d.data(), Dt_d.data(), prev, prev);
        if (cholesky(DtD.data(), prev) != 0) {
            for(int i=0;i<prev;++i) DtD[IDX2(i,i,prev)] += 1e-8;
            if (cholesky(DtD.data(), prev) != 0) {
                fprintf(stderr, "Cholesky failed in empirical_interpolation.\n");
                return -1;
            }
        }
        vector<double> alpha(prev,0.0);
        chol_solve(DtD.data(), Dt_d.data(), alpha.data(), prev);
        int best_p = 0; double best_val = -DBL_MAX;
        for(int p=0;p<N;++p) {
            double approx = 0.0;
            for(int i=0;i<prev;++i) approx += alpha[i] * phi[IDX2(i,p,N)];
            double r = fabs(phi[IDX2(idx_l,p,N)] - approx);
            if (r > best_val) { best_val = r; best_p = p; }
        }
        z_out[idx_l] = best_p;
    }
    return 0;
}

/* ---------- build C and precompute B ---------- */
static void build_C(const double *phi, const int *z, int m, int k, int N, double *C) {
    for(int j=0;j<m;++j) {
        int pj = z[j];
        for(int i=0;i<k;++i) C[IDX2(j,i,k)] = phi[IDX2(i,pj,N)];
    }
}
static int precompute_B(const double *C, int m, int k, double lambda, double *B_out) {
    vector<double> Ct(k*m,0.0), CtC(k*k,0.0);
    transpose(C, Ct.data(), m, k);
    matmul(Ct.data(), C, CtC.data(), k, m, k);
    for(int i=0;i<k;++i) CtC[IDX2(i,i,k)] += lambda;
    if (cholesky(CtC.data(), k) != 0) { fprintf(stderr,"Cholesky failed in precompute_B.\n"); return -1; }
    for(int col=0; col<m; ++col) {
        vector<double> rhs(k,0.0), x(k,0.0);
        for(int r=0;r<k;++r) rhs[r] = Ct[IDX2(r,col,m)];
        chol_solve(CtC.data(), rhs.data(), x.data(), k);
        for(int r=0;r<k;++r) B_out[IDX2(r,col,m)] = x[r];
    }
    return 0;
}

/* ---------- training pipeline ---------- */
static int bpim_train(BPIM &M) {
    compute_mean_and_center(M);
    if (compute_eigenfaces(M) != 0) return -1;
    M.z.assign(M.m, 0);
    if (empirical_interpolation_points(M.phi, M.k, M.N, M.m, M.z) != 0) return -1;
    M.C.assign(M.m * M.k, 0.0);
    build_C(M.phi.data(), M.z.data(), M.m, M.k, M.N, M.C.data());
    M.B.assign(M.k * M.m, 0.0);
    if (precompute_B(M.C.data(), M.m, M.k, 1e-6, M.B.data()) != 0) return -1;
    return 0;
}

/* ---------- gallery build ---------- */
struct Gallery {
    int Kg; int k;
    vector<double> a_hat; // Kg x k
    vector<int> labels;
    double threshold;
};
static int bpim_build_gallery(const BPIM &M, const vector<double> &V, int Kg, Gallery &G) {
    G.Kg = Kg; G.k = M.k;
    G.a_hat.assign(Kg * M.k, 0.0);
    G.labels.assign(Kg,0);
    for(int i=0;i<Kg;++i) {
        vector<double> c(M.m,0.0);
        for(int j=0;j<M.m;++j) c[j] = V[IDX2(i, M.z[j], M.N)];
        double *a_i = &G.a_hat[IDX2(i,0,M.k)];
        matvec(M.B.data(), c.data(), a_i, M.k, M.m);
        G.labels[i] = i;
    }
    G.threshold = 100.0;
    return 0;
}

/* ---------- recognition helpers ---------- */
static void bpim_interpolant_coefficients(const BPIM &M, const double *u, double *a_hat_out) {
    vector<double> c(M.m,0.0);
    for(int j=0;j<M.m;++j) c[j] = u[M.z[j]];
    matvec(M.B.data(), c.data(), a_hat_out, M.k, M.m);
}
static double l2dist(const double *a, const double *b, int n) {
    double s=0.0; for(int i=0;i<n;++i){ double d=a[i]-b[i]; s+=d*d; } return sqrt(s);
}
static int bpim_nearest_neighbor(const Gallery &G, const double *a_q, int *idx_out, double *dist_out) {
    int best=-1; double bd=DBL_MAX;
    for(int i=0;i<G.Kg;++i) {
        const double *a_i = &G.a_hat[IDX2(i,0,G.k)];
        double d = l2dist(a_q, a_i, G.k);
        if (d < bd) { bd = d; best = i; }
    }
    *idx_out = best; *dist_out = bd;
    return 0;
}
static int bpim_recognize(const BPIM &M, const Gallery &G, const double *u_query) {
    vector<double> a_q(M.k,0.0);
    bpim_interpolant_coefficients(M, u_query, a_q.data());
    int idx; double d;
    bpim_nearest_neighbor(G, a_q.data(), &idx, &d);
    if (idx >= 0 && d <= G.threshold) return G.labels[idx];
    return -1;
}

/* ---------- synthetic data generator (same idea as your C code) ---------- */
static void generate_synthetic_faces(vector<double> &U, int K, int N, int persons, int views_per_person) {
    int modes = 10;
    vector<double> basis(modes * N, 0.0);
    for(int m=0;m<modes;++m) for(int p=0;p<N;++p) {
        double x = (double)(p % 90) / 90.0;
        double y = (double)(p / 90) / 74.0;
        double val = sin(2*M_PI*(m+1)*x)*cos(2*M_PI*(m+1)*y);
        basis[IDX2(m,p,N)] = val;
    }
    int idx=0;
    std::mt19937 rng(12345);
    std::uniform_real_distribution<double> ur(-0.5,0.5);
    for(int person=0; person<persons; ++person) {
        double id_bias = (double)person * 0.1;
        for(int v=0; v<views_per_person; ++v) {
            double *img = &U[IDX2(idx,0,N)];
            for(int p=0;p<N;++p) img[p] = 0.0;
            for(int m=0;m<modes;++m) {
                double coeff = ur(rng)*0.5 + id_bias;
                axpy(coeff, &basis[IDX2(m,0,N)], img, N);
            }
            for(int p=0;p<N;++p) img[p] += 0.05 * (ur(rng));
            idx++; if (idx>=K) break;
        }
        if (idx>=K) break;
    }
}

/* ---------- main: train, build gallery, open camera, detect faces, mark and recognize ---------- */
int main() {
    // image dims (paper): 74 x 90 -> N = 6660
    const int W = 90; const int H = 74; const int N = W * H;
    int K = 200;
    int k = 40;
    int m = 2 * k;
    int Kg = 20;

    BPIM M;
    M.N = N; M.K = K; M.k = k; M.m = m;
    M.U.assign(K * N, 0.0);
    M.mean.assign(N, 0.0);

    generate_synthetic_faces(M.U, K, N, 20, K/20);

    if (bpim_train(M) != 0) { fprintf(stderr,"Training failed\n"); return 1; }

    // gallery
    vector<double> V(Kg * N, 0.0);
    generate_synthetic_faces(V, Kg, N, Kg, 1);
    Gallery G;
    bpim_build_gallery(M, V, Kg, G);

    // set threshold heuristic
    double avg=0.0, maxd=0.0;
    for(int i=0;i<G.Kg;++i) {
        int idx; double dist;
        bpim_nearest_neighbor(G, &G.a_hat[IDX2(i,0,G.k)], &idx, &dist);
        avg += dist; if (dist > maxd) maxd = dist;
    }
    avg /= (double)G.Kg;
    G.threshold = avg + (maxd - avg) * 0.5;

    // OpenCV: open camera /dev/video0
    VideoCapture cap(0); // device 0 -> /dev/video0
    if (!cap.isOpened()) { fprintf(stderr,"Cannot open /dev/video0\n"); return 1; }

    // Haar cascade (adjust path if needed)
    string cascade_path = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    CascadeClassifier face_cascade;
    if (!face_cascade.load(cascade_path)) {
        fprintf(stderr,"Cannot load cascade at %s\n", cascade_path.c_str());
        return 1;
    }

    Mat frame;
    namedWindow("BPIM Live", WINDOW_AUTOSIZE);

    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(30,30));

        for (size_t i=0;i<faces.size();++i) {
            Rect r = faces[i];
            // extract face ROI, resize to W x H
            Mat face = gray(r);
            Mat face_resized;
            resize(face, face_resized, Size(W, H)); // width W, height H

            // convert to double vector, flatten row-major
            vector<double> u(N, 0.0);
            for (int y=0;y<H;++y) for (int x=0;x<W;++x) {
                // normalize pixel to roughly same scale as synthetic data
                u[IDX2(0,0,1)]; // no-op to keep macro usage consistent
                double val = (double)face_resized.at<uchar>(y,x) / 255.0;
                u[IDX2(y,x,W)] = val;
            }
            // subtract mean (M.mean)
            for (int p=0;p<N;++p) u[p] -= M.mean[p];

            // compute recognition
            int label = bpim_recognize(M, G, u.data());
            // draw rectangle and label
            Scalar color = (label>=0) ? Scalar(0,255,0) : Scalar(0,0,255);
            rectangle(frame, r, color, 2);
            string text = (label>=0) ? ("ID " + to_string(label)) : "Unknown";
            int baseLine=0;
            Size tsize = getTextSize(text, FONT_HERSHEY_SIMPLEX, 0.6, 1, &baseLine);
            Point tl(r.x, max(0, r.y - 6));
            rectangle(frame, Point(tl.x, tl.y - tsize.height - 4), Point(tl.x + tsize.width, tl.y + baseLine), color, FILLED);
            putText(frame, text, Point(tl.x, tl.y - 2), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255), 1);
        }

        imshow("BPIM Live", frame);
        int key = waitKey(10);
        if (key == 27) break; // ESC to quit
    }

    destroyAllWindows();
    return 0;
}


