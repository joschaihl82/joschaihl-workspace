#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// --- konfiguration ---
#define batch_size 2
#define input_size 2
#define hidden_size 4
#define output_size 1
#define num_epochs 50000
#define learning_rate 0.001f
#define test_interval 1000     // lernfortschritt wird jetzt alle 1000 epochen ausgegeben!
#define reward_factor 0.1f

// --- adamw hyperparameter ---
#define beta1 0.9f
#define beta2 0.999f
#define epsilon 1e-8f
#define weight_decay 0.01f

// --- simuliertes vokabular und daten ---
float important_tokens[] = {1.0f};
#define num_important_tokens (sizeof(important_tokens) / sizeof(important_tokens[0]))
float train_x[][input_size] = { {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f} };
float train_y[] = {0.0f, 1.0f, 1.0f, 0.0f};
#define total_samples (sizeof(train_x) / sizeof(train_x[0]))
#define num_batches (total_samples / batch_size)


// --- modellparameter und optimizer-status ---
typedef struct {
    // schicht 1 gewichte
    float w1[input_size][hidden_size];
    float b1[hidden_size];

    // schicht 2 gewichte
    float w2[hidden_size][output_size];
    float b2[output_size];

    // adamw momente (m = 1. moment, v = 2. moment)
    float m1_w1[input_size][hidden_size]; float v1_w1[input_size][hidden_size];
    float m1_b1[hidden_size];             float v1_b1[hidden_size];
    float m2_w2[hidden_size][output_size]; float v2_w2[hidden_size][output_size];
    float m2_b2[output_size];             float v2_b2[output_size];

} mlp_model_params;

// --- aktivierungsfunktionen ---
float relu(float x) { return x > 0.0f ? x : 0.0f; }
float d_relu(float x) { return x > 0.0f ? 1.0f : 0.0f; }
float sigmoid(float x) { return 1.0f / (1.0f + expf(-x)); }
float d_sigmoid(float x) { float s = sigmoid(x); return s * (1.0f - s); }
int is_important_token(float target) {
    for (int i = 0; i < num_important_tokens; i++) {
        if (fabsf(target - important_tokens[i]) < 1e-6) return 1;
    }
    return 0;
}

// --- initialisierung ---
void init_model(mlp_model_params* model) {
    srand(time(NULL));
    float factor = 0.1f;
    // initialisiere gewichte und momente
    for (int i = 0; i < input_size; i++) {
        for (int j = 0; j < hidden_size; j++) {
            model->w1[i][j] = (float)rand() / (float)rand() * factor;
            model->m1_w1[i][j] = model->v1_w1[i][j] = 0.0f;
        }
    }
    for (int j = 0; j < hidden_size; j++) {
        model->b1[j] = (float)rand() / (float)rand() * factor;
        model->m1_b1[j] = model->v1_b1[j] = 0.0f;
    }
    for (int i = 0; i < hidden_size; i++) {
        for (int j = 0; j < output_size; j++) {
            model->w2[i][j] = (float)rand() / (float)rand() * factor;
            model->m2_w2[i][j] = model->v2_w2[i][j] = 0.0f;
        }
    }
    for (int j = 0; j < output_size; j++) {
        model->b2[j] = (float)rand() / (float)rand() * factor;
        model->m2_b2[j] = model->v2_b2[j] = 0.0f;
    }
}

// --- modell speichern ---
void save_model(mlp_model_params* model, const char* filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fehler beim öffnen von model.bin zum schreiben");
        return;
    }

    fwrite(model->w1, sizeof(float), input_size * hidden_size, fp);
    fwrite(model->b1, sizeof(float), hidden_size, fp);
    fwrite(model->w2, sizeof(float), hidden_size * output_size, fp);
    fwrite(model->b2, sizeof(float), output_size, fp);

    fclose(fp);
    printf("\n✅ adamw-modellgewichte gespeichert in %s (binäres format)\n", filename);
}

// --- lernfortschritt testen (genauigkeit) ---
void test_model(mlp_model_params* model) {
    int correct_predictions = 0;

    for (int i = 0; i < total_samples; i++) {

        float hidden_net[hidden_size];
        float hidden_out[hidden_size];
        float output_net;
        float output_out;

        // forward pass
        for (int j = 0; j < hidden_size; j++) {
            hidden_net[j] = model->b1[j];
            for (int k = 0; k < input_size; k++) { hidden_net[j] += train_x[i][k] * model->w1[k][j]; }
            hidden_out[j] = relu(hidden_net[j]);
        }
        output_net = model->b2[0];
        for (int k = 0; k < hidden_size; k++) { output_net += hidden_out[k] * model->w2[k][0]; }
        output_out = sigmoid(output_net);

        // test-logik: vorhersage > 0.5 wird als 1.0 klassifiziert
        int predicted_class = (output_out > 0.5f) ? 1 : 0;
        int true_class = (int)train_y[i];

        if (predicted_class == true_class) { correct_predictions++; }
    }

    float accuracy = (float)correct_predictions / total_samples * 100.0f;
    printf("| genauigkeit: %.2f%% (korrekt: %d/%d)", accuracy, correct_predictions, total_samples);
}

// --- adamw update funktion ---
void adamw_update(float* param, float* grad, float* m, float* v, long t) {
    // 1. gewichtsabfall (weight decay)
    *param -= learning_rate * weight_decay * (*param);

    // 2. aktualisiere erste und zweite momentenschätzung
    *m = beta1 * (*m) + (1.0f - beta1) * (*grad);
    *v = beta2 * (*v) + (1.0f - beta2) * (*grad) * (*grad);

    // 3. bias-korrektur
    float m_hat = (*m) / (1.0f - powf(beta1, (float)t));
    float v_hat = (*v) / (1.0f - powf(beta2, (float)t));

    // 4. parameter-aktualisierung
    *param -= learning_rate * m_hat / (sqrtf(v_hat) + epsilon);
}


// --- haupttrainingsfunktion ---
void train() {
    mlp_model_params model;
    init_model(&model);
    long step_t = 0; // globaler trainingsschrittzähler für bias-korrektur

    printf("starte adamw mlp training (lr: %.4f, wd: %.3f, test alle %d epochen)...\n", learning_rate, weight_decay, test_interval);

    // gradientenakkumulatoren
    float *grad_w1_acc = (float*)calloc(input_size * hidden_size, sizeof(float));
    float *grad_b1_acc = (float*)calloc(hidden_size, sizeof(float));
    float *grad_w2_acc = (float*)calloc(hidden_size * output_size, sizeof(float));
    float *grad_b2_acc = (float*)calloc(output_size, sizeof(float));

    if (!grad_w1_acc || !grad_b1_acc || !grad_w2_acc || !grad_b2_acc) {
        fprintf(stderr, "speicherzuweisung fehlgeschlagen!\n");
        exit(1);
    }

    // temporärer speicher
    float hidden_net[batch_size][hidden_size];
    float hidden_out[batch_size][hidden_size];
    float output_net[batch_size][output_size];
    float output_out[batch_size][output_size];
    float hidden_delta[batch_size][hidden_size];

    for (int epoch = 0; epoch < num_epochs; epoch++) {
        float epoch_loss = 0.0f;

        for (int b = 0; b < num_batches; b++) {
            step_t++; // globalen schritt inkrementieren

            // gradientenakkumulatoren auf null setzen
            memset(grad_w1_acc, 0, input_size * hidden_size * sizeof(float));
            memset(grad_b1_acc, 0, hidden_size * sizeof(float));
            memset(grad_w2_acc, 0, hidden_size * output_size * sizeof(float));
            memset(grad_b2_acc, 0, output_size * sizeof(float));

            int start_idx = b * batch_size;

            for (int i = 0; i < batch_size; i++) {
                int sample_idx = start_idx + i;
                float target = train_y[sample_idx];

                // 1. forward pass
                for (int j = 0; j < hidden_size; j++) {
                    hidden_net[i][j] = model.b1[j];
                    for (int k = 0; k < input_size; k++) { hidden_net[i][j] += train_x[sample_idx][k] * model.w1[k][j]; }
                    hidden_out[i][j] = relu(hidden_net[i][j]);
                }
                for (int j = 0; j < output_size; j++) {
                    output_net[i][j] = model.b2[j];
                    for (int k = 0; k < hidden_size; k++) { output_net[i][j] += hidden_out[i][k] * model.w2[k][j]; }
                    output_out[i][j] = sigmoid(output_net[i][j]);
                }

                // 2. gewichtete verlustberechnung (dictionary reward)
                float error = target - output_out[i][0];
                float sample_loss = error * error;
                float loss_scale = 1.0f;
                if (is_important_token(target)) {
                    loss_scale = (1.0f + reward_factor);
                    sample_loss *= loss_scale;
                }
                epoch_loss += sample_loss;

                // 3. backward pass (berechnen und akkumulieren)
                float d_loss_out = 2.0f * error * (-1.0f) * loss_scale;
                float output_delta = d_loss_out * d_sigmoid(output_net[i][0]);

                grad_b2_acc[0] += output_delta;
                for (int k = 0; k < hidden_size; k++) { grad_w2_acc[k] += output_delta * hidden_out[i][k]; }

                for (int j = 0; j < hidden_size; j++) {
                    float d_loss_hidden = output_delta * model.w2[j][0];
                    hidden_delta[i][j] = d_loss_hidden * d_relu(hidden_net[i][j]);
                }
                for (int j = 0; j < hidden_size; j++) {
                    grad_b1_acc[j] += hidden_delta[i][j];
                    for (int k = 0; k < input_size; k++) {
                        grad_w1_acc[k * hidden_size + j] += hidden_delta[i][j] * train_x[sample_idx][k];
                    }
                }
            } // ende stichproben-schleife

            // 4. adamw optimierungsschritt (einmal pro batch)
            float scale = 1.0f / batch_size;

            float grad_b2_norm[1] = {grad_b2_acc[0] * scale};
            adamw_update(&model.b2[0], &grad_b2_norm[0], &model.m2_b2[0], &model.v2_b2[0], step_t);

            for (int k = 0; k < hidden_size; k++) {
                float grad_w2_norm[1] = {grad_w2_acc[k] * scale};
                adamw_update(&model.w2[k][0], &grad_w2_norm[0], &model.m2_w2[k][0], &model.v2_w2[k][0], step_t);
            }

            for (int j = 0; j < hidden_size; j++) {
                float grad_b1_norm[1] = {grad_b1_acc[j] * scale};
                adamw_update(&model.b1[j], &grad_b1_norm[0], &model.m1_b1[j], &model.v1_b1[j], step_t);

                for (int k = 0; k < input_size; k++) {
                    int idx = k * hidden_size + j;
                    float grad_w1_norm[1] = {grad_w1_acc[idx] * scale};
                    adamw_update(&model.w1[k][j], &grad_w1_norm[0], &model.m1_w1[k][j], &model.v1_w1[k][j], step_t);
                }
            }
        } // ende batch-schleife

        // --- lernfortschritt ausgeben ---
        if (epoch % test_interval == 0) {
            printf("epoche %5d | gew. durchschn. verlust: %.8f", epoch, epoch_loss / total_samples);
            test_model(&model); // test-funktion gibt genauigkeit aus
            printf("\n");
        }
    } // ende epochen-schleife

    // speicher freigeben
    free(grad_w1_acc); free(grad_b1_acc);
    free(grad_w2_acc); free(grad_b2_acc);

    save_model(&model, "model.bin");
}

int main() {
    train();
    return 0;
}
