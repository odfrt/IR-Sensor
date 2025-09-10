# IR-Sensor
Software para a detecção de obstáculos utilizando um sensor Infravermelho de reflexão ligado ao kit de desenvolvimento STM32MP1-DK1, baseado no processador STM32MP1.

## Instruções de uso

### Compilação
Para compilar o programa, é necessário utilizar uma toolchain que inclui o compilador cruzado _**arm-buildroot-linux-gnueabihf-g++**_, possibilitando a criação de um executável compatível com a arquitetura da placa. 

Para descompactar a toolchain, execute o seguinte comando:

```
tar -xvf arm-buildroot-linux-gnueabihf_sdk-DK2.tar.gz
```

Com a pasta da toolchain descompactada, podemos utilizar o comando abaixo para compilar o executável.
```
arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-g++ --sysroot=arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot -Wall -O2 src/main.cpp -o IRSensor
```

### Conectando à placa

Com o executável compilado, podemos prosseguir para o uso da placa. Primeiramente, é necessário conectá-la à alimentação via cabo USB-C. 

A conexão com a placa poderá ser feita de duas formas. A primeira das duas é utilizando um cabo micro-USB conectado à porta serial da placa. Conecte o cabo ao seu computador e utilize o software _TeraTerm_ para acessar o terminal da placa.

A segunda forma é via conexão wireless. Primeiro, conecte a placa ao seu roteador via cabo ethernet. Conecte seu computador ao Wi-Fi e configure um endereço IPV4 estático na mesma faixa da placa (192.168.42.xx.}
Feito isso, abra o seu terminal ou Powershell e execute o comando `ssh root@<ip da placa>` e digite a senha. Agora, você tem acesso ao terminal da placa de maneira remota.

### Transferindo arquivos

O segundo método permite também a transferência de arquivos via SCP. Para isso, novamente no terminal ou Powershell de seu computador, execute 
```
scp -O IRSensor root@192.168.42.2:/
```

Agora, o executável estará localizado no diretório root da placa. Caso não possua permissão para executar o programa, digite `chmod +x IRSensor` no terminal da placa.

### Execução

Conecte o sensor à placa (Atente-se aos jumpers, não faça a conexão de forma errada!) e execute o programa. Se tudo ocorrer de forma correta, o terminal deverá mostrar o seguinte: