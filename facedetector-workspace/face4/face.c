/*
  BPIM: Best Points Interpolation Method – Vollständige C-Implementierung
  -----------------------------------------------------------------------
  Enthält:
    - PCA/Eigenfaces (kleines G = (1/K) U^T U, Jacobi-Eigenzerlegung)
    - Rückprojektion in Eigenfaces, Normierung
    - Empirical Interpolation: Auswahl m bester Pixel (greedy residual)
    - Cholesky-Zerlegung für SPD-Lösung (C^T C)^{-1} C^T
    - Galerieaufbau und Erkennung (Nearest Neighbor in k-dim Raum)
    - Demo-Daten (synthetisch)

  Kompilieren:
    gcc -O2 -std=c11 bpim.c -o bpim
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

/* ---------- Utility ---------- */

#define IDX2(r,c,nc) ((r)*(nc) + (c))

static void *xcalloc(size_t n, size_t sz) {
    void *p = calloc(n, sz);
    if (!p) { fprintf(stderr, "Out of memory\n"); exit(1); }
    return p;
}
static void *xmalloc(size_t sz) {
    void *p = malloc(sz);
    if (!p) { fprintf(stderr, "Out of memory\n"); exit(1); }
    return p;
}

/* ---------- Vektor/Matrix-Tools ---------- */

static void matmul(const double *A, const double *B, double *C, int rA, int cA, int cB) {
    for (int i = 0; i < rA; ++i) {
        for (int j = 0; j < cB; ++j) {
            double s = 0.0;
            for (int k = 0; k < cA; ++k) s += A[IDX2(i,k,cA)] * B[IDX2(k,j,cB)];
            C[IDX2(i,j,cB)] = s;
        }
    }
}
static void matvec(const double *A, const double *x, double *y, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        double s = 0.0;
        for (int j = 0; j < cols; ++j) s += A[IDX2(i,j,cols)] * x[j];
        y[i] = s;
    }
}
static void transpose(const double *A, double *B, int r, int c) {
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            B[IDX2(j,i,r)] = A[IDX2(i,j,c)];
}
static double dot(const double *a, const double *b, int n) {
    double s = 0.0; for (int i = 0; i < n; ++i) s += a[i]*b[i]; return s;
}
static double norm2(const double *a, int n) {
    double s = 0.0; for (int i = 0; i < n; ++i) s += a[i]*a[i]; return sqrt(s);
}
static void scale(double *a, int n, double alpha) {
    for (int i = 0; i < n; ++i) a[i] *= alpha;
}
static void axpy(double alpha, const double *x, double *y, int n) {
    for (int i = 0; i < n; ++i) y[i] += alpha * x[i];
}

/* ---------- Cholesky-Zerlegung für SPD ---------- */

static int cholesky(double *A, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= i; ++j) {
            double s = A[IDX2(i,j,n)];
            for (int k = 0; k < j; ++k)
                s -= A[IDX2(i,k,n)] * A[IDX2(j,k,n)];
            if (i == j) {
                if (s <= 1e-15) return -1;
                A[IDX2(i,j,n)] = sqrt(s);
            } else {
                A[IDX2(i,j,n)] = s / A[IDX2(j,j,n)];
            }
        }
        for (int j = i+1; j < n; ++j) A[IDX2(i,j,n)] = 0.0; /* obere auffüllen: L nur unten */
    }
    return 0;
}
static void chol_solve(const double *L, const double *b, double *x, int n) {
    /* Vorwärtseinsetzen: L y = b */
    double *y = (double*)xmalloc(sizeof(double)*n);
    for (int i = 0; i < n; ++i) {
        double s = b[i];
        for (int k = 0; k < i; ++k) s -= L[IDX2(i,k,n)] * y[k];
        y[i] = s / L[IDX2(i,i,n)];
    }
    /* Rückwärtseinsetzen: L^T x = y */
    for (int i = n-1; i >= 0; --i) {
        double s = y[i];
        for (int k = i+1; k < n; ++k) s -= L[IDX2(k,i,n)] * x[k];
        x[i] = s / L[IDX2(i,i,n)];
    }
    free(y);
}

/* ---------- Jacobi-Eigenzerlegung für symmetrische Matrix ---------- */

static void jacobi_eigendecompose(double *A, double *eigvecs, double *eigvals, int n, int max_iter, double tol) {
    /* A wird in-place diagonalisiert; eigvecs init auf Identität */
    for (int i = 0; i < n; ++i) {
        eigvals[i] = A[IDX2(i,i,n)];
        for (int j = 0; j < n; ++j)
            eigvecs[IDX2(i,j,n)] = (i == j) ? 1.0 : 0.0;
    }
    for (int it = 0; it < max_iter; ++it) {
        /* finde größten Off-Diagonal */
        int p = 0, q = 1;
        double max_off = 0.0;
        for (int i = 0; i < n; ++i)
            for (int j = i+1; j < n; ++j) {
                double val = fabs(A[IDX2(i,j,n)]);
                if (val > max_off) { max_off = val; p = i; q = j; }
            }
        if (max_off < tol) break;
        double app = A[IDX2(p,p,n)], aqq = A[IDX2(q,q,n)], apq = A[IDX2(p,q,n)];
        double phi = 0.5 * atan2(2.0 * apq, (aqq - app));
        double c = cos(phi), s = sin(phi);

        /* Update A */
        for (int i = 0; i < n; ++i) {
            double aip = A[IDX2(i,p,n)], aiq = A[IDX2(i,q,n)];
            A[IDX2(i,p,n)] = c * aip - s * aiq;
            A[IDX2(i,q,n)] = s * aip + c * aiq;
        }
        for (int i = 0; i < n; ++i) {
            double api = A[IDX2(p,i,n)], aqi = A[IDX2(q,i,n)];
            A[IDX2(p,i,n)] = c * api - s * aqi;
            A[IDX2(q,i,n)] = s * api + c * aqi;
        }
        A[IDX2(p,q,n)] = 0.0;
        A[IDX2(q,p,n)] = 0.0;

        /* Update eigvecs */
        for (int i = 0; i < n; ++i) {
            double vip = eigvecs[IDX2(i,p,n)], viq = eigvecs[IDX2(i,q,n)];
            eigvecs[IDX2(i,p,n)] = c * vip - s * viq;
            eigvecs[IDX2(i,q,n)] = s * vip + c * viq;
        }
    }
    /* Nach Iterationen: Diagonalwerte als Eigenwerte auslesen */
    for (int i = 0; i < n; ++i) eigvals[i] = A[IDX2(i,i,n)];
}

/* ---------- Datenstrukturen ---------- */

typedef struct {
    int N;    /* Pixel */
    int K;    /* Trainingsbilder */
    int k;    /* Eigenfaces */
    int m;    /* beste Pixel */
    double *U;    /* K x N, mean-subtracted */
    double *mean; /* N */
    double *phi;  /* k x N, Eigenfaces */
    int *z;       /* m Pixel-Indizes */
    double *C;    /* m x k */
    double *B;    /* k x m */
} BPIM_Model;

typedef struct {
    int Kg;
    int k;
    double *a_hat; /* Kg x k */
    int *labels;   /* Kg */
    double threshold;
} BPIM_Gallery;

/* ---------- Mittelwert und Zentrierung ---------- */

static void compute_mean_and_center(double *U, double *mean, int K, int N) {
    memset(mean, 0, sizeof(double)*N);
    for (int i = 0; i < K; ++i)
        for (int p = 0; p < N; ++p)
            mean[p] += U[IDX2(i,p,N)];
    for (int p = 0; p < N; ++p) mean[p] /= (double)K;
    for (int i = 0; i < K; ++i)
        for (int p = 0; p < N; ++p)
            U[IDX2(i,p,N)] -= mean[p];
}

/* ---------- PCA/Eigenfaces ---------- */

static int compute_eigenfaces(BPIM_Model *M) {
    /* G = (1/K) U^T U   (K x K) */
    double *G = (double*)xcalloc(M->K * M->K, sizeof(double));
    double invK = 1.0 / (double)M->K;
    /* U: K x N, wir brauchen U^T U (K x K): sum_p U[i,p] * U[j,p] */
    for (int i = 0; i < M->K; ++i) {
        for (int j = 0; j < M->K; ++j) {
            double s = 0.0;
            for (int p = 0; p < M->N; ++p)
                s += M->U[IDX2(i,p,M->N)] * M->U[IDX2(j,p,M->N)];
            G[IDX2(i,j,M->K)] = invK * s;
        }
    }
    /* Jacobi: Eigenwerte/-vektoren von G */
    double *eigvecsG = (double*)xcalloc(M->K * M->K, sizeof(double));
    double *eigvalsG = (double*)xcalloc(M->K, sizeof(double));

    /* Kopie von G, da Jacobi in-place arbeitet */
    double *Gcopy = (double*)xmalloc(sizeof(double) * M->K * M->K);
    memcpy(Gcopy, G, sizeof(double) * M->K * M->K);

    jacobi_eigendecompose(Gcopy, eigvecsG, eigvalsG, M->K, 1000, 1e-10);

    /* Sortiere nach absteigenden Eigenwerten */
    int *idx = (int*)xmalloc(sizeof(int) * M->K);
    for (int i = 0; i < M->K; ++i) idx[i] = i;
    for (int i = 0; i < M->K-1; ++i) {
        int max_i = i;
        for (int j = i+1; j < M->K; ++j)
            if (eigvalsG[j] > eigvalsG[max_i]) max_i = j;
        if (max_i != i) {
            double tmpv = eigvalsG[i]; eigvalsG[i] = eigvalsG[max_i]; eigvalsG[max_i] = tmpv;
            int tmpi = idx[i]; idx[i] = idx[max_i]; idx[max_i] = tmpi;
        }
    }

    /* Rückprojektion: phi_i = U v_i  (v_i ist Eigenvektor von G) */
    M->phi = (double*)xcalloc(M->k * M->N, sizeof(double));
    for (int i = 0; i < M->k; ++i) {
        int sel = idx[i];
        const double *v_i = &eigvecsG[IDX2(0, sel, M->K)]; /* Spalte sel */

        /* temp = U^T * v_i? Wir wollen phi_i(p) = sum_{l=1..K} U[l,p] * v_i[l] */
        for (int p = 0; p < M->N; ++p) {
            double s = 0.0;
            for (int l = 0; l < M->K; ++l)
                s += M->U[IDX2(l,p,M->N)] * v_i[l];
            M->phi[IDX2(i,p,M->N)] = s;
        }
        /* Normierung */
        double nrm = norm2(&M->phi[IDX2(i,0,M->N)], M->N);
        if (nrm > 1e-15) scale(&M->phi[IDX2(i,0,M->N)], M->N, 1.0/nrm);
    }

    free(G); free(Gcopy); free(eigvecsG); free(eigvalsG); free(idx);
    return 0;
}

/* ---------- Empirical Interpolation: beste Pixel ---------- */

static int empirical_interpolation_points(const double *phi, int k, int N, int m, int *z_out) {
    if (m < 1) return -1;
    /* z_1: max |phi_1| */
    int z1 = 0; double max_abs = 0.0;
    for (int p = 0; p < N; ++p) {
        double v = fabs(phi[IDX2(0,p,N)]);
        if (v > max_abs) { max_abs = v; z1 = p; }
    }
    z_out[0] = z1;

    /* l = 2..m: residualbasiert */
    for (int l = 2; l <= m; ++l) {
        int idx_l = l - 1;
        int prev = (idx_l < k) ? idx_l : k;

        /* D(prev x prev) und d(prev) für Projektion phi_l @ z_j auf Basis 1..prev */
        double *D = (double*)xcalloc(prev * prev, sizeof(double));
        double *d = (double*)xcalloc(prev, sizeof(double));
        for (int j = 0; j < prev; ++j) {
            int zj = z_out[j];
            d[j] = phi[IDX2(idx_l, zj, N)];
            for (int i = 0; i < prev; ++i) {
                D[IDX2(j,i,prev)] = phi[IDX2(i, zj, N)];
            }
        }
        /* Normalgleichungen: (D^T D) alpha = D^T d */
        double *Dt = (double*)xcalloc(prev * prev, sizeof(double));
        double *DtD = (double*)xcalloc(prev * prev, sizeof(double));
        double *Dt_d = (double*)xcalloc(prev, sizeof(double));
        transpose(D, Dt, prev, prev);
        matmul(Dt, D, DtD, prev, prev, prev);
        matvec(Dt, d, Dt_d, prev, prev);

        /* Cholesky */
            if (cholesky(DtD, prev) != 0) {
            /* leichte Regularisierung */
            for (int i = 0; i < prev; ++i) DtD[IDX2(i,i,prev)] += 1e-8;
            if (cholesky(DtD, prev) != 0) {
                fprintf(stderr, "Cholesky failed in empirical_interpolation.\n");
                free(D); free(d); free(Dt); free(DtD); free(Dt_d);
                return -1;
            }
        }
        double *alpha = (double*)xcalloc(prev, sizeof(double));
        chol_solve(DtD, Dt_d, alpha, prev);

        /* bestes Pixel: max |phi_l(p) - sum(alpha_i phi_i(p))| */
        int best_p = 0; double best_val = -DBL_MAX;
        for (int p = 0; p < N; ++p) {
            double approx = 0.0;
            for (int i = 0; i < prev; ++i) approx += alpha[i] * phi[IDX2(i,p,N)];
            double r = fabs(phi[IDX2(idx_l,p,N)] - approx);
            if (r > best_val) { best_val = r; best_p = p; }
        }
        z_out[idx_l] = best_p;

        free(D); free(d); free(Dt); free(DtD); free(Dt_d); free(alpha);
    }
    return 0;
}

/* ---------- C und B ---------- */

static void build_C(const double *phi, const int *z, int m, int k, int N, double *C) {
    for (int j = 0; j < m; ++j) {
        int pj = z[j];
        for (int i = 0; i < k; ++i)
            C[IDX2(j,i,k)] = phi[IDX2(i, pj, N)];
    }
}

static int precompute_B(const double *C, int m, int k, double lambda, double *B_out) {
    double *Ct = (double*)xcalloc(k * m, sizeof(double));
    double *CtC = (double*)xcalloc(k * k, sizeof(double));
    transpose(C, Ct, m, k);
    matmul(Ct, C, CtC, k, m, k);
    for (int i = 0; i < k; ++i) CtC[IDX2(i,i,k)] += lambda;

    if (cholesky(CtC, k) != 0) {
        fprintf(stderr, "Cholesky failed in precompute_B.\n");
        free(Ct); free(CtC); return -1;
    }
    /* inv(CtC) * Ct: Spaltenweise lösen: (CtC) X = Ct */
    for (int col = 0; col < m; ++col) {
        double *rhs = (double*)xcalloc(k, sizeof(double));
        double *x = (double*)xcalloc(k, sizeof(double));
        for (int r = 0; r < k; ++r) rhs[r] = Ct[IDX2(r,col,m)];
        chol_solve(CtC, rhs, x, k);
        for (int r = 0; r < k; ++r) B_out[IDX2(r,col,m)] = x[r];
        free(rhs); free(x);
    }
    free(Ct); free(CtC);
    return 0;
}

/* ---------- Training ---------- */

static int bpim_train(BPIM_Model *M) {
    /* Mittelwert und Zentrierung */
    compute_mean_and_center(M->U, M->mean, M->K, M->N);

    /* PCA/Eigenfaces */
    if (compute_eigenfaces(M) != 0) return -1;

    /* beste Pixel */
    M->z = (int*)xmalloc(sizeof(int) * M->m);
    if (empirical_interpolation_points(M->phi, M->k, M->N, M->m, M->z) != 0) return -1;

    /* C */
    M->C = (double*)xcalloc(M->m * M->k, sizeof(double));
    build_C(M->phi, M->z, M->m, M->k, M->N, M->C);

    /* B */
    M->B = (double*)xcalloc(M->k * M->m, sizeof(double));
    if (precompute_B(M->C, M->m, M->k, 1e-6, M->B) != 0) return -1;

    return 0;
}

/* ---------- Galerie ---------- */

static int bpim_build_gallery(const BPIM_Model *M, const double *V, int Kg, BPIM_Gallery *G) {
    G->Kg = Kg; G->k = M->k;
    G->a_hat = (double*)xcalloc(Kg * M->k, sizeof(double));
    G->labels = (int*)xmalloc(sizeof(int) * Kg);

    for (int i = 0; i < Kg; ++i) {
        double *c = (double*)xmalloc(sizeof(double) * M->m);
        for (int j = 0; j < M->m; ++j) c[j] = V[IDX2(i, M->z[j], M->N)]; /* Intensität an z_j */
        double *a_i = &G->a_hat[IDX2(i,0,M->k)];
        matvec(M->B, c, a_i, M->k, M->m);
        G->labels[i] = i;
        free(c);
    }
    G->threshold = 100.0; /* später validieren/tunen */
    return 0;
}

/* ---------- Erkennung ---------- */

static void bpim_interpolant_coefficients(const BPIM_Model *M, const double *u, double *a_hat_out) {
    double *c = (double*)xmalloc(sizeof(double) * M->m);
    for (int j = 0; j < M->m; ++j) c[j] = u[M->z[j]];
    matvec(M->B, c, a_hat_out, M->k, M->m);
    free(c);
}
static double l2dist(const double *a, const double *b, int n) {
    double s = 0.0; for (int i = 0; i < n; ++i) { double d = a[i]-b[i]; s += d*d; } return sqrt(s);
}
static int bpim_nearest_neighbor(const BPIM_Gallery *G, const double *a_q, int *idx_out, double *dist_out) {
    int best = -1; double bd = DBL_MAX;
    for (int i = 0; i < G->Kg; ++i) {
        const double *a_i = &G->a_hat[IDX2(i,0,G->k)];
        double d = l2dist(a_q, a_i, G->k);
        if (d < bd) { bd = d; best = i; }
    }
    *idx_out = best; *dist_out = bd;
    return 0;
}
static int bpim_recognize(const BPIM_Model *M, const BPIM_Gallery *G, const double *u_query) {
    double *a_q = (double*)xcalloc(M->k, sizeof(double));
    bpim_interpolant_coefficients(M, u_query, a_q);
    int idx; double d;
    bpim_nearest_neighbor(G, a_q, &idx, &d);
    free(a_q);
    if (idx >= 0 && d <= G->threshold) return G->labels[idx];
    return -1;
}

/* ---------- Demo-Daten (synthetisch) ---------- */

static void generate_synthetic_faces(double *U, int K, int N, int persons, int views_per_person) {
    /* Erzeuge K Gesichter als lineare Kombination weniger Grundmuster + Rauschen */
    int modes = 10;
    double *basis = (double*)xcalloc(modes * N, sizeof(double));
    /* zufällige Basis mit glatten Flecken */
    for (int m = 0; m < modes; ++m) {
        for (int p = 0; p < N; ++p) {
            double x = (double)(p % 90) / 90.0;
            double y = (double)(p / 90) / 74.0;
            double val = sin(2*M_PI*(m+1)*x)*cos(2*M_PI*(m+1)*y);
            basis[IDX2(m,p,N)] = val;
        }
    }
    int idx = 0;
    for (int person = 0; person < persons; ++person) {
        double id_bias = (double)person * 0.1;
        for (int v = 0; v < views_per_person; ++v) {
            double *img = &U[IDX2(idx,0,N)];
            for (int p = 0; p < N; ++p) img[p] = 0.0;
            for (int m = 0; m < modes; ++m) {
                double coeff = ((double)rand()/RAND_MAX - 0.5) * 0.5 + id_bias;
                axpy(coeff, &basis[IDX2(m,0,N)], img, N);
            }
            for (int p = 0; p < N; ++p)
                img[p] += 0.05 * (((double)rand()/RAND_MAX) - 0.5);
            idx++;
            if (idx >= K) break;
        }
        if (idx >= K) break;
    }
    free(basis);
}

/* ---------- Hauptprogramm ---------- */

int main(void) {
    /* Bildgröße wie im Paper-Beispiel: 74 x 90 => N = 6660 */
    int N = 74 * 90;
    int K = 200;      /* Trainingsanzahl */
    int k = 40;       /* Eigenfaces (anpassbar) */
    int m = 2 * k;    /* Interpolationspixel */
    int Kg = 20;      /* Galeriegröße */

    /* Allokation Modell */
    BPIM_Model M = {0};
    M.N = N; M.K = K; M.k = k; M.m = m;
    M.U = (double*)xcalloc(K * N, sizeof(double));
    M.mean = (double*)xcalloc(N, sizeof(double));

    /* Synthetische Trainingsdaten erzeugen */
    generate_synthetic_faces(M.U, K, N, 20, K/20);

    /* Trainieren */
    if (bpim_train(&M) != 0) {
        fprintf(stderr, "Training fehlgeschlagen.\n");
        return 1;
    }

    /* Galerie-Daten (synthetisch) */
    double *V = (double*)xcalloc(Kg * N, sizeof(double));
    generate_synthetic_faces(V, Kg, N, Kg, 1);

    BPIM_Gallery G = {0};
    if (bpim_build_gallery(&M, V, Kg, &G) != 0) {
        fprintf(stderr, "Galerieaufbau fehlgeschlagen.\n");
        return 1;
    }

    /* Threshold grob setzen: mittlere intra-/inter-Distanzen schätzen */
    /* Einfache Heuristik: 95%-Quantil der Distanzen zum NN für Galerie selbst */
    double avg = 0.0; double maxd = 0.0;
    for (int i = 0; i < Kg; ++i) {
        int idx; double dist;
        bpim_nearest_neighbor(&G, &G.a_hat[IDX2(i,0,G.k)], &idx, &dist);
        avg += dist; if (dist > maxd) maxd = dist;
    }
    avg /= (double)Kg;
    G.threshold = avg + (maxd - avg) * 0.5;

    /* Query erzeugen (Galerie + Rauschen) */
    double *u_query = (double*)xcalloc(N, sizeof(double));
    memcpy(u_query, &V[IDX2(3,0,N)], sizeof(double)*N);
    for (int p = 0; p < N; ++p) u_query[p] += 0.02 * (((double)rand()/RAND_MAX) - 0.5);

    int label = bpim_recognize(&M, &G, u_query);
    if (label >= 0) printf("Erkannt: Label = %d (Distanz <= Threshold)\n", label);
    else            printf("Unbekanntes Gesicht (Distanz > Threshold)\n");

    /* Cleanup */
    free(M.U); free(M.mean); free(M.phi);
    free(M.z); free(M.C); free(M.B);
    free(V); free(G.a_hat); free(G.labels);
    free(u_query);

    return 0;
}

