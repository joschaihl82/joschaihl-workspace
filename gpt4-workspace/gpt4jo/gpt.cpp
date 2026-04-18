#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer> // For exponential backoff simulation

// --- Configuration ---
// NOTE: Replace "YOUR_OPEN_GPT_API_KEY" with your actual key.
const QString API_KEY = "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";
// Assuming a standard chat completion endpoint structure.
const QUrl API_ENDPOINT("http://api.opengpt.com/v1/chat/completions");

// --- End Configuration ---

/**
 * @brief The ChatWindow class sets up the GUI and handles the API communication.
 *
 * It contains a QTextEdit for history, a QLineEdit for input, and a QPushButton for sending.
 */
class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        // Set up the main window properties
        setWindowTitle("OpenGPT Qt Chat Client");
        resize(800, 600);

        // --- Widgets ---
        // 1. Rich Text Component for History
        historyEdit = new QTextEdit;
        historyEdit->setReadOnly(true);
        historyEdit->setHtml("<p style='color: gray; font-style: italic;'>Welcome! Start chatting below.</p>");

        // 2. Input Box for Chat Message
        inputLine = new QLineEdit;
        inputLine->setPlaceholderText("Type your message here...");

        // 3. Send Button
        sendButton = new QPushButton("Send");
        sendButton->setFixedSize(100, 35);

        // Network manager initialization
        networkManager = new QNetworkAccessManager(this);

        // --- Layout ---
        QWidget *centralWidget = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Add history (takes most of the space)
        mainLayout->addWidget(historyEdit);

        // Horizontal layout for input and button
        QHBoxLayout *inputLayout = new QHBoxLayout;
        inputLayout->addWidget(inputLine);
        inputLayout->addWidget(sendButton);

        // Add input area to the main layout
        mainLayout->addLayout(inputLayout);

        setCentralWidget(centralWidget);

        // --- Connections ---
        // Connect the button click to the sendMessage slot
        connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);

        // Connect the Enter key press in the line edit to the sendMessage slot
        connect(inputLine, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);

        // Connect the network manager's signal to the handler slot
        connect(networkManager, &QNetworkAccessManager::finished,
                this, &ChatWindow::handleNetworkReply);
    }

private slots:
    /**
     * @brief Reads the user input, updates the UI, and initiates the API request.
     */
    void sendMessage()
    {
        QString userMessage = inputLine->text().trimmed();

        if (userMessage.isEmpty()) {
            return;
        }

        // 1. Update History with User Message
        historyEdit->append(QString("<b>You:</b> %1").arg(userMessage));

        // 2. Clear input and disable controls
        inputLine->clear();
        inputLine->setEnabled(false);
        sendButton->setEnabled(false);
        historyEdit->append("<p style='color: blue;'>... Thinking ...</p>"); // Loading indicator

        // 3. Prepare API Request
        QNetworkRequest request(API_ENDPOINT);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        // Authorization header using Bearer token (common for most LLM APIs)
        request.setRawHeader("Authorization", QString("Bearer %1").arg(API_KEY).toUtf8());

        // Construct the JSON payload (assuming OpenAI-like structure)
        QJsonObject message;
        message["role"] = "user";
        message["content"] = userMessage;

        QJsonArray messages;
        messages.append(message);

        QJsonObject payload;
        payload["model"] = "gemini-2.5-flash"; // Placeholder model name
        payload["messages"] = messages;

        QJsonDocument doc(payload);
        QByteArray data = doc.toJson();

        // 4. Send the POST request
        qDebug() << "Sending request to API...";
        networkManager->post(request, data);
    }

    /**
     * @brief Handles the response from the API server.
     * @param reply The network reply object.
     */
    void handleNetworkReply(QNetworkReply *reply)
    {
        // Remove the loading indicator
        // This is a simple approach; for robustness, track the last line written.
        QTextCursor cursor(historyEdit->document());
        cursor.movePosition(QTextCursor::End);
        cursor.select(QTextCursor::BlockUnderCursor);
        if (cursor.selectedText().contains("... Thinking ...")) {
            cursor.removeSelectedText();
        }

        if (reply->error() != QNetworkReply::NoError) {
            QString errorMsg = QString("API Error: %1. Please check your API key and endpoint.").arg(reply->errorString());
            historyEdit->append(QString("<p style='color: red;'><b>System:</b> %1</p>").arg(errorMsg));
            qCritical() << "Network Error:" << reply->errorString();
        } else {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

            if (jsonResponse.isObject()) {
                QJsonObject root = jsonResponse.object();

                // Assuming the response structure contains "choices" array
                if (root.contains("choices") && root["choices"].isArray()) {
                    QJsonArray choices = root["choices"].toArray();
                    if (!choices.isEmpty()) {
                        QJsonObject firstChoice = choices.first().toObject();
                        QJsonObject message = firstChoice["message"].toObject();

                        if (message.contains("content") && message["content"].isString()) {
                            QString assistantResponse = message["content"].toString();

                            // 5. Update History with Assistant Message (Rich Text)
                            historyEdit->append(QString("<b>Assistant:</b> %1").arg(assistantResponse.toHtmlEscaped()));

                        } else {
                            historyEdit->append("<p style='color: red;'><b>System:</b> Could not parse content from API response.</p>");
                            qCritical() << "Failed to parse content from JSON:" << responseData;
                        }
                    } else {
                        historyEdit->append("<p style='color: red;'><b>System:</b> Empty choices array in API response.</p>");
                    }
                } else if (root.contains("error")) {
                    QJsonObject error = root["error"].toObject();
                    QString errorMsg = error.contains("message") ? error["message"].toString() : "Unknown API error.";
                    historyEdit->append(QString("<p style='color: red;'><b>System:</b> API Error: %1</p>").arg(errorMsg));
                    qCritical() << "API Returned Error:" << responseData;
                } else {
                    historyEdit->append("<p style='color: red;'><b>System:</b> Received unexpected API response format.</p>");
                    qCritical() << "Unexpected JSON structure:" << responseData;
                }
            } else {
                historyEdit->append("<p style='color: red;'><b>System:</b> Failed to parse JSON response.</p>");
                qCritical() << "Failed to parse JSON document:" << responseData;
            }
        }

        // Clean up the reply object
        reply->deleteLater();

        // 6. Re-enable controls
        inputLine->setEnabled(true);
        sendButton->setEnabled(true);
        inputLine->setFocus();
    }

private:
    QTextEdit *historyEdit;
    QLineEdit *inputLine;
    QPushButton *sendButton;
    QNetworkAccessManager *networkManager;
};

// Required for MOC (Meta-Object Compiler) system to work with slots/signals
#include "gpt.moc"

int main(int argc, char *argv[])
{
    // The Q_OBJECT macro used in ChatWindow requires an external file (main.moc)
    // to be generated by the moc tool during compilation. We include it here.
    // If you compile this using a standard Qt build system (qmake or CMake),
    // this #include is often automatically handled/generated.

    QApplication app(argc, argv);

    // Check for API key placeholder
    if (API_KEY == "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") {
        qCritical() << "FATAL: Please replace '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' with your actual API key in the source code.";
        // Display a simple error message to the user
        QMainWindow errorWindow;
        errorWindow.setWindowTitle("Configuration Error");
        QTextEdit errorText;
        errorText.setHtml("<p style='color: red; font-size: 16px; padding: 20px;'><b>Configuration Error:</b><br><br>Please edit the source file (main.cpp) and replace <code>YOUR_OPEN_GPT_API_KEY</code> with your actual API key.</p>");
        errorText.setReadOnly(true);
        errorWindow.setCentralWidget(&errorText);
        errorWindow.resize(400, 200);
        errorWindow.show();
        // Return here, do not attempt to run the main application
        // return app.exec(); // Or just let the error window be the main loop
        // The error window will be destroyed when main returns.
    }

    ChatWindow window;
    window.show();

    return app.exec();
}
