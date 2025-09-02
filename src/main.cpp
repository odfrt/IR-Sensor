#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

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

public:
    /**
    * @brief Construtor da classe IRSensor.
    * @param adcPath Caminho do arquivo que fornece a leitura do ADC.
    */
    IRSensor(const string& adcPath) {
        path = adcPath;
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
        file >> value;
        return value;
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
            if (obstacle) {
                cout << "\nHá obstáculo!" << endl;
            }
            else {
                cout << "\nNão há obstáculo!" << endl;
            }
            cout << "Saída do sensor: " << value << endl;

            sleep(1);
        }
    }
};

int main() {
    IRSensor ir_sensor ("/sys/bus/iio/devices/iio:device0/in_voltage13_raw");
    ir_sensor.detectObstacle();
    return 0;
};
