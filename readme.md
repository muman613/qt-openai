# qt-openai

`qt-openai` is a C++ library that provides an intuitive interface for interacting with OpenAI's API using the Qt framework. This project leverages the power of OpenAI's language models while utilizing Qt's robust networking and JSON capabilities to create seamless integrations for your Qt applications.

## Features

- **Asynchronous API Calls**: Leverages Qt's `QNetworkAccessManager` for non-blocking API requests.
- **Simple Interface**: Provides a clean and easy-to-use interface for making requests to OpenAI's API.
- **JSON Parsing**: Parses and handles JSON responses efficiently using `QJsonDocument`.
- **Extendable**: Designed to be extended for additional OpenAI API endpoints.

## Requirements

- Qt 5.15+ or Qt 6.x
- C++17 or later
- A valid OpenAI API key

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/muman613/qt-openai.git
   cd qt-openai
   ```

2. Include the project in your Qt application:
   - Add the library source files to your project.
   - Update your `.pro` file or CMake configuration to include the necessary headers and sources.

3. Install dependencies:
   - Ensure your Qt installation supports `QNetworkAccessManager` and `QJsonDocument`.

## Usage

1. **Set up the library in your project**:
   Include the `OpenAIClient` class and related files.

2. **Initialize the client**:
   ```cpp
   #include "OpenAIClient.h"

   OpenAIClient client("your_openai_api_key");
   ```

3. **Make a request**:
   ```cpp
   // Example: Sending a prompt to OpenAI's ChatGPT API
   QString prompt = "What is the capital of France?";
   client.sendChatRequest(prompt, [](const QString& response) {
       qDebug() << "OpenAI Response:" << response;
   });
   ```

4. **Handle responses**:
   Use the provided callback mechanism or signals/slots for handling responses.

## Examples

- See the [examples](examples/) directory for sample applications demonstrating various use cases.

## Contributing

Contributions are welcome! If you find a bug or have a feature request, please open an issue or submit a pull request.

### How to Contribute

1. Fork the repository.
2. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add your message here"
   ```
4. Push your branch and open a pull request:
   ```bash
   git push origin feature/your-feature-name
   ```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [OpenAI](https://openai.com/) for providing the API and inspiration.
- The Qt Project for the robust C++ framework that powers this library.

---

Thank you for using `qt-openai`. If you have any questions, feel free to open an issue or contact the maintainer.
