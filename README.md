## Problema 2 - Interfaces de E/S

### Autores
<div align="justify">
    <li><a href="https://github.com/dlucasafj">@dlucasafj</a></li>
    <li><a href="https://github.com/ian-zaque">@ian-zaque</a></li>
</div>

<h1>Contextualização do Problema</h1>
Em continuação do <a href="https://github.com/ian-zaque/pbl_SD_1">Problema 1</a>, um sistema de leitura de sensores genéricos deve ser implementado. Os programas a serem implementados devem ser capazes de ler até 8 sensores digitais e um sensor analógico, verificando estes através do módulo Wifi ESP8266 NodeMcu. O sistema deve ser comandado, de forma automatizada, por um Single Board Computer (SBC) que neste contexto será a Raspberry Pi Zero W.

A comunicação será iniciada pelo SBC que envia pela UART (Universal Asynchronous Receiver / Transmitter) os dados de requisição para o NodeMcu serialmente que responde adequadamente seguindo o protocolo de comandos. Além de monitorar sensores e manipular o LED imbutido do NodeMcu, o sistema deve exibir no display anexo as informações respectivas a requisição realizada. É possível ler o problema completo <a href="./Problema 2 - Interfaces de E-S.pdf">neste link</a>.

<h1>MQTT</h1>
O protocolo MQTT foi proposto pelo Dr. Andy Stanford-Clark, da IBM, e Arlen Nipper (Eurotech) em 1999. Com o objetivo de desenvolver um protocolo aberto, simples e fácil de implementar. No entanto, o protocolo só se tornou público em 2010 na versão 3.1, disponibilizada pela IBM, e em 2014 veio a se tornar um padrão OASIS com sua versão 3.1.1. 

O MQTT é um protocolo de camada de aplicação que opera sobre o protocolo TCP na camada de transporte. Foi desenvolvido para ser aplicado em ambientes com redes e dispositivos restritos. Seu funcionamento é baseado em uma arquitetura de publicação/assinatura. Neste tipo de arquitetura existem dois elementos principais: o broker e o cliente. O broker pode ser visto como um controlador e é o responsável por coordenar o fluxo de mensagem através dos tópicos, que podem ser vistos como endereços de memória especiais. Os clientes são os nós finais, responsáveis por gerar e consumir os dados. 

O funcionamento de sistemas MQTT funciona da seguinte forma: Os clientes se anunciam ao broker, indicando qual é a sua função (publicar ou assinar) e qual o tópico. Em sequência, o broker cria o tópico e gerencia o fluxo de mensagens entre o publicador e o assinante. Tudo feito sobre o protocolo TCP na camada de transporte. 

A comunicação dentro de um sistema MQTT é feita através de troca de mensagens. Essas mensagens do protocolo são compostas por três campos: cabeçalho fixo, variável e o payload. O cabeçalho fixo tem o tamanho fixo de 2 bytes e está sempre presente nas mensagens. Este cabeçalho tem a finalidade de comportar as informações da mensagem como, o tipo, flags de configuração e o tamanho máximo da mensagem. O cabeçalho variável comporta informações de controle, para sinalizar as ações que o broker deve executar. Este cabeçalho existe em alguns tipos de mensagens e está localizado entre o cabeçalho fixo e o payload. Por fim, a mensagem MQTT apresenta um payload, no qual os dados estão contidos. Uma mensagem MQTT pode ter um tamanho máximo de 256 MB. Vale ressaltar que apenas o cabeçalho fixo é obrigatório nas mensagens, os outros campos podem variar de acordo com o tipo da mensagem. 

O protocolo MQTT disponibiliza três níveis de QoS, que é uma ferramenta que permite determinar quais dispositivos e serviços terão maior prioridade de conexão. Uma mensagem MQTT pode ser configurada para assumir um desses níveis, de acordo com seu nível de confiabilidade de envio e recebimento das mensagens. O nível zero, garante apenas o envio, não se preocupando com o recebimento. O nível um, exige que o recebimento seja confirmado pelo menos uma vez, não impedindo múltiplos recebimentos. O nível dois, garante que a mensagem seja recebida exatamente uma vez, impedindo múltiplos recebimentos. 

O protocolo MQTT apresenta diversas vantagens para IoT, IIoT, e entre essas vantagens esta o baixo consumo de recursos, onde o MQTT tem um baixo consumo de memória, havendo então uma pouca necessidade de um processamento forte para que o transporte de mensagens seja realizado, também há um baixo consumo de banda. A alta confiabilidade, que é feita través de configurações do QoS e sua alta segurança, onde suas mensagens são protegidas através do uso de certificados SSL, o MQTT suporta diversos tipos de mecanismos para proteção de informações e de autenticações que podem ser configurados com facilidade pelo cliente diretamente no Broker. 


<h1>Protocolos de Comunicação</h1>
A comunicação entre o SBC e o NodeMcu segue um padrão definido no corpo do problema, contendo 2 bytes por mensagem. Um byte de requisição que especifica qual função será executada e um byte de endereço do sensor que é desejado ser lido. Quando não há um sensor a ser lido, o segundo byte é dado como 0. Nas tabelas abaixo é possível ver o protocolo de requisição e o de resposta.

<div align="justify">
    <img src="./images/tabela1Reqs.png">
</div>
<div align="justify">
    <small>Fonte: Problema 2 - Interfaces de E/S</small>
</div>

O protocolo de requisição ainda conta com mais uma requisição: <b> Desligamento do LED do NodeMcu (Código 0x07) </b>.
<br>

<div align="justify">
    <img src="./images/tabela2Resps.png">
</div>
<div align="justify">
    <small>Fonte: Problema 2 - Interfaces de E/S</small>
</div>

<h1>Single Board Computer (SBC) </h1>
SBC trata-se de um computador de placa única. Nele encontram-se presentes um processador, memória e dispositivos de entrada e saída. São uma solução rápida e integrada de software e hardware e amplamente utilizados para comunicação entre dispositivos muito específicos como sistemas robóticos e controladores de processos.
O SBC, na figura do Raspberry Pi Zero W, foi configurado para utilizar UART através de um programa escrito em linguagem C utilizando as bibliotecas wiringPi e wiringSerial. Ambas tratam de maneira simplificada o uso de portas/dispositivos seriais. Para que o programa utilize UART é necessário identificar qual dispositivo de saída será usado e definir o Baud Rate em 9600. As funções da biblioteca foram encapsuladas em métodos presentes no código do SBC de modo que pudessem ser operadas de forma mais adequada ao problema. Todas as instruções da biblioteca podem ser encontradas <a href="http://wiringpi.com/reference/serial-library/">aqui</a>, porém na solução foram utilizadas somente as seguintes:

```sh
    int serialOpen (char *device, int baud_rate);
```
A função acima é responsável inicializa o dispositivo serial e define o Baud Rate. O valor da porta serial será retornado em caso de sucesso e, caso encontre algum erro, o valor retornado será -1.

```sh
    void serialPuts (int serial_port, char *stringMsg);
```
Esta função escreve serialmente a mensagem string (stringMsg) na porta (serial_port).

```sh
    int serialGetchar (int serial_port);
```
A função serialGetChar retorna o próximo caractér a ser lido no dispositivo serial. Seu parâmetro (serial_port) indica de onde deve ler. Caso encontre erro, é retornado -1.

```sh
    void serialFlush (int serial_port);
```
Esta função elimina e descarta todo dado recebido ou em espera para ser enviado pela porta serial (serial_port).


<h1>Diagrama e Arquitetura</h1>
Com os programas devidamente compilados e presentes nas plataformas, o SBC inicia a comunicação enviando dois bytes. O primeiro será o comando da requisição e o segundo o endereço do sensor. A estrutura do programa implementado segue um loop de 5 iterações que é reiniciado ao chegar na última para que o fluxo seja automatizado. O corpo do loop verifica o índice atual e segue o padrão:

1. Definição dos bytes de requisição em varíaveis locais de tipo vetor de caractéres.
2. Descarte de dados previamente recebidos.
3. Envio dos bytes para o NodeMcu.
4. Recebimento dos bytes de resposta do NodeMcu.
5. Verificação de resposta e escrita no display.

No índice 0, é enviado a primeira requisição: solicitação do status do NodeMcu.
No índice 1, a segunda: solicitação de valor do sensor analógico.
No índice 2, a terceira: solicitação de valor de um sensor digital. A seleção do sensor digital se dá através da iteração de 1 a 9 onde cada item solicita o sensor respectivo.
No índice 3, é enviado a quarta requisição: acendimento de LED imbutido do NodeMcu.
No índice 4, a quinta requisição: desligamento do LED imbutido.

O diagrama a seguir resume o fluxo da solução do problema:

<div align="justify">
    <img src="./images/arquitetura.png">
</div>

<br>

<h1>Máquinas e Ferramentas</h1>

1. Hardware:
    - Raspberry Pi Zero W;
    - ESP8266 NodeMcu ESP-12E Module;
2. Arquitetura Raspberry:
    - ARMv6;
3. Linguagens de programação: 
    - Assembly;
    - C;
4. Software:
    - Arduino IDE

<h1> Instruções de uso </h1>

1. Clone o repositório.
    ```sh
        git clone https://github.com/ian-zaque/pbl_SD_2.git
    ```

2. Compile os programas.
    ```sh
        make
    ```

3. Configure o módulo Wifi ESP8266 NodeMcu.
    * No software Arduino IDE:
    <ul>
        <li> Abrir arquivo SD_PBL.ino </li>
        <li> Verificar conexão e porta Wifi do ESP NodeMcu </li>
        <li> Se conectado, carregar sketch no módulo </li>
    </ul

4. Execute o arquivo abaixo gerado.
    ```sh
        sudo ./uart.run
     ```
