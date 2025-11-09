#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>

using namespace std;

/**
 * @brief Classe para leitura e interpretação de valores do sensor IR.
 * @details Esta classe acessa o valor de saída do sensor e determina se há
 * obstáculos com base em um limiar predefinido.
 */
class IRSensor {
private:
    /**
    * @brief Caminho do arquivo a ser lido.
    */
    string path;
    /**
     * @brief Limiar do ADC para detecção de obstáculos.
     * @details O arquivo lido retorna o valor 65535 para indicar a voltagem 3.3V
     * (sem obstáculo) e valores baixos (em torno de 3000) caso contrário.
     * Para levar em conta flutuações, é adotado o limiar de 60000, garantindo uma margem de erro grande.
     */
    const int ADCThreshold = 60000;
    /** @brief Socket UDP para envio de dados. */
    int sockfd;
    /** @brief Estrutura do endereço do servidor destino. */
    struct sockaddr_in dest_addr;

public:
    /**
     * @brief Construtor da classe IRSensor.
     * @param adcPath Caminho do arquivo que fornece a leitura do ADC.
     * @param serverIP Endereço IP do servidor UDP.
     * @param serverPort Porta do servidor UDP.
     */
    IRSensor(const string& adcPath, const string& serverIP, int serverPort) {
        path = adcPath;

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("ERRO NO SOCKET");
            exit(EXIT_FAILURE);
        }

        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIP.c_str(), &dest_addr.sin_addr) <= 0) {
            cerr << "IP INVALIDO" << endl;
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Destrutor da classe IRSensor.
     * @details Fecha o socket.
     */
    ~IRSensor() {
        close(sockfd);
    }

    /**
     * @brief Lê o valor do ADC a partir do arquivo.
     * @return Valor inteiro lido do ADC ou -1 em caso de erro.
     */
    int readADC() {
        ifstream file(path);
        if (!file) {
        return -1;
        }
        int value;
        if (!(file >> value)) return -1;
        return value;
    }

    /**
     * @brief Envia os dados do sensor para o servidor via UDP.
     * @param adcValue Valor lido do ADC.
     * @param status Status do sensor ("OBSTRUCTED" ou "CLEAR").
     */
    void sendData(int adcValue, const string& status) {
        string message = "\nADC =" + to_string(adcValue) + " | STATUS =" + status;
        ssize_t sent = sendto(sockfd, message.c_str(), message.size(), 0,
                              (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (sent < 0) perror("ERRO NO ENVIO");
    }

    /**
     * @brief Executa a detecção contínua de obstáculos.
     * @details Realiza um loop, lendo o ADC e utilizando o limiar definido para identificar se há ou não obstáculo.
     */
    void detectObstacle() {
        while (true) {
            int value = readADC();
            if (value == -1) {
                cerr << "\nErro na leitura do sensor!" << endl;
                sleep(1);
                continue;
            }

            bool obstacle = (value <= ADCThreshold);
            string status = obstacle ? "OBSTRUCTED" : "CLEAR";

            cout << "STATUS: " << status << " | ADC: " << value << endl;

            sendData(value, status);

            usleep(300000);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "USO: " << argv[0] << " <server_ip> <server_port>\n";
        cerr << "EXEMPLO: " << argv[0] << " 192.168.42.10 5000\n";
        return 1;
    }

    string serverIP = argv[1];
    int serverPort = atoi(argv[2]);

    IRSensor ir_sensor ("/sys/bus/iio/devices/iio:device0/in_voltage13_raw", serverIP, serverPort);

    cout << "ENVIANDO PARA: " << serverIP << ":" << serverPort << "...\n";

    ir_sensor.detectObstacle();

    return 0;
}
