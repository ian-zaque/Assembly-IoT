## Problema 3 - IOT

### Autores
<div align="justify">
    <li><a href="https://github.com/dlucasafj">@dlucasafj</a></li>
    <li><a href="https://github.com/ian-zaque">@ian-zaque</a></li>
</div>

<h1>Indice</h1>
<ol>
    <li>Contextualização do Problema</li>
    <li>Fundamentação Teórica </li>
    2.1. Protocolo de Comunicação <br>
        2.1.1. MQTT <br>
        2.1.2. HTTP <br>
    2.2. SBC <br>
    2.3. Esp8266-NodeMCU
    <li>Resultados e Discussão</li>
    3.1 Arquitetura do Sistema <br>
    3.2 Comunicação SBC - ESP <br>
    3.3 Comunicação SBC - Interface Web
    <li>Conclusão e Considerações
    <li>Referências</li>
    
    
    
</ol>


<h1>Contextualização do Problema</h1>
Em continuação do <a href="https://github.com/ian-zaque/pbl_SD_1">Problema 1</a>, um sistema de leitura de sensores genéricos deve ser implementado. Os programas a serem implementados devem ser capazes de ler até 8 sensores digitais e um sensor analógico, verificando estes através do módulo Wifi ESP8266 NodeMcu. O sistema deve ser comandado, de forma automatizada, por um Single Board Computer (SBC) que neste contexto será a Raspberry Pi Zero W.

A comunicação será iniciada pelo SBC que envia pela UART (Universal Asynchronous Receiver / Transmitter) os dados de requisição para o NodeMcu serialmente que responde adequadamente seguindo o protocolo de comandos. Além de monitorar sensores e manipular o LED imbutido do NodeMcu, o sistema deve exibir no display anexo as informações respectivas a requisição realizada. É possível ler o problema completo <a href="./Problema 2 - Interfaces de E-S.pdf">neste link</a>.

<h1>Fundamentação Teórica</h1>
<h2>Protocolos de Comunicação</h2>
<h4>MQTT</h4>
O protocolo MQTT foi proposto pelo Dr. Andy Stanford-Clark, da IBM, e Arlen Nipper (Eurotech) em 1999. Com o objetivo de desenvolver um protocolo aberto, simples e fácil de implementar. No entanto, o protocolo só se tornou público em 2010 na versão 3.1, disponibilizada pela IBM, e em 2014 veio a se tornar um padrão OASIS com sua versão 3.1.1. 

O MQTT é um protocolo de camada de aplicação que opera sobre o protocolo TCP na camada de transporte. Foi desenvolvido para ser aplicado em ambientes com redes e dispositivos restritos. Seu funcionamento é baseado em uma arquitetura de publicação/assinatura. Neste tipo de arquitetura existem dois elementos principais: o broker e o cliente. O broker pode ser visto como um controlador e é o responsável por coordenar o fluxo de mensagem através dos tópicos, que podem ser vistos como endereços de memória especiais. Os clientes são os nós finais, responsáveis por gerar e consumir os dados. 

O funcionamento de sistemas MQTT funciona da seguinte forma: Os clientes se anunciam ao broker, indicando qual é a sua função (publicar ou assinar) e qual o tópico. Em sequência, o broker cria o tópico e gerencia o fluxo de mensagens entre o publicador e o assinante. Tudo feito sobre o protocolo TCP na camada de transporte. 

A comunicação dentro de um sistema MQTT é feita através de troca de mensagens. Essas mensagens do protocolo são compostas por três campos: cabeçalho fixo, variável e o payload. O cabeçalho fixo tem o tamanho fixo de 2 bytes e está sempre presente nas mensagens. Este cabeçalho tem a finalidade de comportar as informações da mensagem como, o tipo, flags de configuração e o tamanho máximo da mensagem. O cabeçalho variável comporta informações de controle, para sinalizar as ações que o broker deve executar. Este cabeçalho existe em alguns tipos de mensagens e está localizado entre o cabeçalho fixo e o payload. Por fim, a mensagem MQTT apresenta um payload, no qual os dados estão contidos. Uma mensagem MQTT pode ter um tamanho máximo de 256 MB. Vale ressaltar que apenas o cabeçalho fixo é obrigatório nas mensagens, os outros campos podem variar de acordo com o tipo da mensagem. 

O protocolo MQTT disponibiliza três níveis de QoS, que é uma ferramenta que permite determinar quais dispositivos e serviços terão maior prioridade de conexão. Uma mensagem MQTT pode ser configurada para assumir um desses níveis, de acordo com seu nível de confiabilidade de envio e recebimento das mensagens. O nível zero, garante apenas o envio, não se preocupando com o recebimento. O nível um, exige que o recebimento seja confirmado pelo menos uma vez, não impedindo múltiplos recebimentos. O nível dois, garante que a mensagem seja recebida exatamente uma vez, impedindo múltiplos recebimentos. 

O protocolo MQTT apresenta diversas vantagens para IoT, IIoT, e entre essas vantagens esta o baixo consumo de recursos, onde o MQTT tem um baixo consumo de memória, havendo então uma pouca necessidade de um processamento forte para que o transporte de mensagens seja realizado, também há um baixo consumo de banda. A alta confiabilidade, que é feita través de configurações do QoS e sua alta segurança, onde suas mensagens são protegidas através do uso de certificados SSL, o MQTT suporta diversos tipos de mecanismos para proteção de informações e de autenticações que podem ser configurados com facilidade pelo cliente diretamente no Broker. 

<h4>HTTP</h4>
HTTP é um protocolo que permite a obtenção de recursos, como documentos HTML. É a base de qualquer troca de dados na Web e um protocolo cliente-servidor, o que significa que as requisições são iniciadas pelo destinatário, geralmente um navegador Web. Um documento é reconstruído a partir dos diferentes subdocumentos obtidos, como por exemplo texto, descrição do layout, imagens, vídeos, scripts entre outros. 
Clientes e servidores se comunicam trocando mensagens individuais, As mensagens enviadas pelo cliente, são chamadas de solicitações(requests), ou requisições, e as mensagens enviadas pelo servidor como resposta são chamadas de respostas(responses). 

<h2>SBC</h2>
Um Single-Board Computer (SBC) é um microcomputador construído sobre uma placa única, possuindo um microprocessador, memória de acesso randômico (RAM) e suporte para dispositivos de entrada/saída.  A simplicidade de sua arquitetura contribui para um baixo consumo de energia e boa performance de CPU. Atualmente, os SBCs têm sido utilizados em diversos ramos como o de instrumentação médica, computação embarcada e indústria automobilística. Seu tamanho e custo reduzido são as principais vantagens para utilizá-los quando comparados aos tradicionais computadores do tipo desktop.

<h2>ESP8266-NodeMCU</h2>
O módulo ESP8266 é um microcontrolador da empresa Espressif que trata-se basicamente de um WiFI-SOC, ou seja, ele possui a capacidade de se conectar a uma rede WiFi, pode atuar como uma aplicação stand-alone, onde ele não precisa de nenhum outro componente para funcionar, ou como um servidor escravo MCU (Microcontroller Unit). Na segunda condição ele funciona como um adaptador WiFi para outro microcontrolador, como por exemplo arduino. 
Em operação o ESP8266 pode funcionar em duas configurações, são elas: access point e client. Na primeira configuração, ele funciona basicamente como um roteador, criando uma rede WiFi restrita por login e senha. Como client, ele estabelece uma conexão com a rede WiFi escolhida, uma vez conectado também cria um servidor e todos os dispositivos conectados na mesma rede WiFi que o ESP8266 têm acesso a este servidor pelo endereço de IP.
Node MCU é um microcontrolador que surgiu para facilitar a construção de projetos utilizando o ESP8266, pois apresenta um regulador de tensão de 3,3V, e com ele não é mais necessário a utilização de outros componentes como adaptadores SPI e UART para realizar a conexão. Ele surgiu logo após o lançamento do ESP8266, sendo lançado com o intuito de ser uma placa para desenvolvimento de projetos de caráter IoT.  O NodeMCU pode ser programado através de scripts escritos na linguagem LUA, ou pode ser programado através da linguagem C++, pela própria plataforma do arduino. 


<h1>Resultados e Discussão</h1>
<h2>Arquitetura do Sistema</h2>
O diagrama a seguir resume o fluxo da solução do problema:

<div align="justify">
    <img src="./images/arquitetura.png">
</div>
<h2>Comunicação SBC-ESP</h2>
<h2>Comunicação SBC-Interface Web</h2>

<h1>Conclusão e Considerações</h1>
<h1>Referências</h1>

