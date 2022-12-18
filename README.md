## Problema 3 - IoT: A Internet das Coisas

### Autores
<div align="justify">
    <li><a href="https://github.com/dlucasafj">@dlucasafj</a></li>
    <li><a href="https://github.com/ian-zaque">@ian-zaque</a></li>
</div>

<details>
<summary>Instruções de uso</summary>

1. Clone o repositório.
    ```sh
        git clone https://github.com/ian-zaque/pbl_SD_3.git
    ```

2. No SBC, compilar o arquivo mqtt.c
    ```sh
        make mqtt
    ```

3. Configure o módulo Wifi ESP8266 NodeMcu.
    * No software Arduino IDE:
    <ul>
        <li> Abrir arquivo PBL_SD_3.ino </li>
        <li> Verificar conexão e porta Wifi do ESP NodeMCU </li>
        <li> Se conectado, carregar sketch no módulo </li>
    </ul

4. Navegar até a pasta */api* e executar:
    ```sh
        npm install
    ```
* Após conclusão executar:
    ```sh
        node server.js
    ```

5. Para executar a IHM Web (interface), suba servidor HTTP local.

6. Navegar até a pasta */interface_html* e abrir arquivo *index.html*
    * Recomenda-se utilzar a IDE VsCode e sua extensão LIVE-SERVER


</details>

<h1>Índice</h1>
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
A internet das coisas (IoT) trata-se de uma rede de objetos físicos do dia-a-dia que, junto a outros dispositivos e sensores, conseguem trocar informações e dados relevantes com outros programas e objetos. Com intuito de evoluir o <a href="https://github.com/ian-zaque/pbl_SD_2">Problema 2</a>, um novo sistema deverá ser implementado com modificações de modo a atender os requisitos presentes anteriormente mas também adequar o programa a IoT, trazendo novas funcionalidades.

Os programas a serem implementados devem ser capazes de ler até 8 sensores digitais, um sensor analógico, verificando estes através do módulo Wifi ESP8266 NodeMCU e interagir com um LED. O sistema deve ser comandado, de forma automatizada, por um Single Board Computer (SBC) que neste contexto será a Orange Pi PC Plus.

A comunicação será iniciada pelo SBC que envia via MQTT (Message Queue Telemetry Transport) os dados de solicitação, por meio de tópicos específicos, para o NodeMCU que responde adequadamente seguindo o protocolo de comandos. Além de monitorar sensores e manipular o LED imbutido do NodeMCU, o sistema deve exibir no display anexo as informações respectivas a requisição realizada, a resposta obtida. Também deve ser incluso na solução do problema uma interface homem-máquina (IHM) que deve exibir os dados dos sensores em formato de gráficos com histórico das 10 últimas medições. Nesta mesma IHM o status da NodeMCU deve constar indicando sua conexão. É possível ler o problema completo <a href="./Problema 3 - IoT A Internet das Coisas.pdf">neste link</a>.

<h1>Fundamentação Teórica</h1>
<h2>Protocolos de Comunicação</h2>
<h4>MQTT</h4>
O protocolo MQTT foi proposto pelo Dr. Andy Stanford-Clark, da IBM, e Arlen Nipper (Eurotech) em 1999. Com o objetivo de desenvolver um protocolo aberto, simples e fácil de implementar. No entanto, o protocolo só se tornou público em 2010 na versão 3.1, disponibilizada pela IBM, e em 2014 veio a se tornar um padrão OASIS com sua versão 3.1.1. 

O MQTT é um protocolo de camada de aplicação que opera sobre o protocolo TCP na camada de transporte. Foi desenvolvido para ser aplicado em ambientes com redes e dispositivos restritos. Seu funcionamento é baseado em uma arquitetura de publicação/assinatura. Neste tipo de arquitetura existem dois elementos principais: o broker e o cliente. O broker pode ser visto como um controlador e é o responsável por coordenar o fluxo de mensagem através dos tópicos, que podem ser vistos como endereços de memória especiais onde as mensagens são enviadas. Os clientes são os nós finais, responsáveis por gerar e consumir os dados.

O funcionamento de sistemas MQTT funciona da seguinte forma: Os clientes se anunciam ao broker, indicando qual é a sua função (publicar ou assinar) e qual o tópico. Em sequência, o broker cria o tópico e gerencia o fluxo de mensagens entre o publicador e o assinante. Tudo feito sobre o protocolo TCP na camada de transporte. 

A comunicação dentro de um sistema MQTT é feita através de troca de mensagens. Essas mensagens do protocolo são compostas por três campos: cabeçalho fixo, variável e o payload. O cabeçalho fixo tem o tamanho fixo de 2 bytes e está sempre presente nas mensagens. Este cabeçalho tem a finalidade de comportar as informações da mensagem como, o tipo, flags de configuração e o tamanho máximo da mensagem. O cabeçalho variável comporta informações de controle, para sinalizar as ações que o broker deve executar. Este cabeçalho existe em alguns tipos de mensagens e está localizado entre o cabeçalho fixo e o payload. Por fim, a mensagem MQTT apresenta um payload, no qual os dados estão contidos. Uma mensagem MQTT pode ter um tamanho máximo de 256 MB. Vale ressaltar que apenas o cabeçalho fixo é obrigatório nas mensagens, os outros campos podem variar de acordo com o tipo da mensagem. 

O protocolo MQTT disponibiliza três níveis de QoS (Quality of Service), que é uma ferramenta que permite determinar quais dispositivos e serviços terão maior prioridade de conexão. Uma mensagem MQTT pode ser configurada para assumir um desses níveis, de acordo com seu nível de confiabilidade de envio e recebimento das mensagens. O nível zero, garante apenas o envio, não se preocupando com o recebimento. O nível um, exige que o recebimento seja confirmado pelo menos uma vez, não impedindo múltiplos recebimentos. O nível dois, garante que a mensagem seja recebida exatamente uma vez, impedindo múltiplos recebimentos. 

O protocolo MQTT apresenta diversas vantagens para IoT, IIoT, e entre essas vantagens esta o baixo consumo de recursos, onde o MQTT tem um baixo consumo de memória, havendo então pouca necessidade de um processamento forte para que o transporte de mensagens seja realizado, também há um baixo consumo de banda. A alta confiabilidade, que é feita através de configurações do QoS e sua alta segurança, onde suas mensagens são protegidas através do uso de certificados SSL, o MQTT suporta diversos tipos de mecanismos para proteção de informações e de autenticações que podem ser configurados com facilidade pelo cliente diretamente no Broker. 

<h4>HTTP</h4>
HTTP (Hypertext Transfer Protocol) é um protocolo que permite a obtenção de recursos como documentos HTML. É a base de qualquer troca de dados na Web e um protocolo cliente-servidor, o que significa que as requisições são iniciadas pelo destinatário, geralmente um navegador Web. Um documento é reconstruído a partir dos diferentes subdocumentos obtidos, como por exemplo texto, descrição do layout, imagens, vídeos, scripts entre outros. 
Clientes e servidores se comunicam trocando mensagens individuais. As mensagens enviadas pelo cliente são chamadas de solicitações (requests) ou requisições, e as mensagens enviadas pelo servidor como resposta são chamadas de respostas (responses). 

<h2>SBC</h2>
Um Single-Board Computer (SBC) é um microcomputador construído sobre uma placa única possuindo um microprocessador, memória de acesso randômico (RAM) e suporte para dispositivos de entrada/saída. A simplicidade de sua arquitetura contribui para um baixo consumo de energia e boa performance de CPU. Atualmente, os SBCs têm sido utilizados em diversos ramos como o de instrumentação médica, computação embarcada e indústria automobilística. Seu tamanho e custo reduzido são as principais vantagens para utilizá-los quando comparados aos tradicionais computadores do tipo desktop. São uma solução rápida e integrada de software e hardware e amplamente utilizados para comunicação entre dispositivos muito específicos como sistemas robóticos e controladores de processos. Na solução do 3º problema foi utilizado a Orange Pi PC Plus.

<h2>ESP8266-NodeMCU</h2>
O módulo ESP8266 é um microcontrolador da empresa Espressif que trata-se basicamente de um WiFI-SOC, ou seja, ele possui a capacidade de se conectar a uma rede WiFi, pode atuar como uma aplicação stand-alone, onde ele não precisa de nenhum outro componente para funcionar, ou como um servidor escravo MCU (Microcontroller Unit). Na segunda condição ele funciona como um adaptador WiFi para outro microcontrolador, como por exemplo Arduino. 
Em operação o ESP8266 pode funcionar em duas configurações, são elas: access point e client. Na primeira configuração, ele funciona basicamente como um roteador, criando uma rede WiFi restrita por login e senha. Como client, ele estabelece uma conexão com a rede WiFi escolhida, uma vez conectado também cria um servidor e todos os dispositivos conectados na mesma rede WiFi que o ESP8266 têm acesso a este servidor pelo endereço de IP.
Node MCU é um microcontrolador que surgiu para facilitar a construção de projetos utilizando o ESP8266, pois apresenta um regulador de tensão de 3.3V, e com ele não é mais necessário a utilização de outros componentes como adaptadores SPI e UART para realizar a conexão. Ele surgiu logo após o lançamento do ESP8266, sendo lançado com o intuito de ser uma placa para desenvolvimento de projetos de caráter IoT. O NodeMCU pode ser programado através de scripts escritos na linguagem LUA, ou pode ser programado através da linguagem C++, pela própria plataforma do Arduino (Arduino Sketch). 


<h1>Resultados e Discussão</h1>
<h2>Arquitetura do Sistema</h2>
O diagrama a seguir resume o fluxo da solução do problema:

![Imagem de detalhamento da arquitetura implementada para solução](./images/arquitetura.png)

<h2>Comunicação SBC - ESP</h2>
O funcionamento do código da SBC opera na seguinte ordem:

1. Definição de bibliotecas, variáveis globais, estruturas e protótipos de funções.
2. Definição de variáveis locais, configuração MQTT e LCD Display.
3. Inicialização IHM Display, IHM Terminal.
4. Envio e recebimento automático de comandos e respostas via MQTT.
5. Verificação e tratamento de respostas.  
6. Atualização de histórico de medições dos sensores.
7. Publicação de dados dos sensores e NodeMCU para IHM Web.

###### Passo 1
O ponto 1 engloba apenas configurações gerais mínimas para o funcionamento do programa. Foram feitas as inclusões de bibliotecas de entrada e saída, manipulação de string, temporização, uma específica para interação com o display LCD. Utilizou-se ainda de duas bibliotecas não-nativas que precisaram ser instalados globalmente no dispositivo: 
*wiringPi.h* e a de utlização do protocolo MQTT _MQTTClient.h_. Logo abaixo foram definidas as variáveis globais de composição da blbioteca MQTT, os tópicos para SBC trocar mensagem e os pinos para iniciaçização do Display LCD. 

Foram criadas também uma variável global de tipo MQTTClient, oriunda da blblioteca de mesmo nome, e uma de tipo int chamada *display_lcd*. Em seguida foi definida uma estrutura (Struct) de nome Historic, contendo neste registro 4 campos: um campo char para guardar o nome do sensor a ser tratado, 2 vetores de 10 posições cada um de tipo int. Um dos vetores chamado historic guarda os valores das dez últimas medições do sensor em questão e o outro armazena o tempo em milissegundos da medição respectiva. 

Foi instanciado um vetor de 10 posições do tipo struct Historic, uma posição para cada sensor de modo que será inicializado posteriormente na função ``` initArrayRegistros(); ```. Por último, nesta primeira seção, estão os protótipos das funções implementadas (a definição, comentários, documentação e corpo das funções está abaixo, depois do fim da função principal).

![Imagem do struct Historic](./images/struct_historic.png)

![Imagem do protótipo das funções](./images/prototipo_funcoes.png)

###### Passo 2
No ponto 2, já dentro da função main(), estão definidas as variáveis locais usadas para estruturação da conexão MQTT. Nesta seção, fazendo uso da biblioteca *MQTTClient.h *, as configuração do cliente MQTT são criadas e passadas: as funções de reconexão, recebimento de mensagens dos tópicos, ID do cliente SBC, usuáriom senha de autenticação e endereço do Broker. Feita a configuração e havendo conexão com o Broker, pode-se iniciar a subscrição de tópicos e escrita neles. Caso não haja conexão o programa é encerrado, visto que é necessário para o funcionamento geral que haja troca de dados via MQTT. 
Com o MQTT configurado e subscrito no tópico, o próximo passo é inicializar o LCD Display. É realizado o setup de entradas e saídas, através de *wiringPi.h* e *wiringPiDev.h* e em seguida a inicialização dos pinos. O último passo então é inicializar o vetor de histórico *array_registros[10]*, como citado anteriormente.

###### Passo 3
Em seguida, no passo 3, a IHM Display é ativada e começa a exibir as mensagens de apresentação do programa tanto no display quanto no terminal (IHM Terminal) por meio de textos impressos na tela. Assim como a solução do problema 2, as solicitação para o NodeMCU são realizadas automaticamente através de um loop for() que publica no tópico *NODEMCU_PUBLISH* o comando específico da ação da iteração, seguindo o protocolo definido antes.

O protocolo de requisição ainda conta com mais uma requisição: Desligamento do LED do NodeMcu (Código 0x07).
![Protocolo de comandos de solicitação para NodeMCU](./images/tabela1Reqs.png)

###### Passo 4
Neste ponto, o loop for já está iterando e os comandos são enviados na ordem que são apresentadas na tabela 1. As mensagens são enviadas através da função _publish(MQTTClient, char *, char *)_. Existe, porém, uma diferença realizada para a solicitação de sensores digitais: A mensagem enviada possui 2 dígitos, o primeiro indicando que se trata de um sensor digital e o segundo dígito qual sensor digital está sendo requistado valor. Uma outra sutileza nesse loop é que ao chegar no índice 4, o último visto que a condicional do laço de repetição é o índice seja menor ou igual a 4, ele é atualizado para -1, assim o loop será reiniciado e voltará a posição 0 e reiniciará a publicação de mensagens de solicitação.

###### Passo 5
Enquanto o tópico *NODEMCU_RECEIVE* não recebe mensagens, os comandos continuam a ser enviados regularmente. Caso receba alguma mensagem, a função *on_message(void *, char *, int, MQTTClient_message *)*, que foi passada para a configuração MQTT que a todo instante verifica o recebimento, persiste o payload recebido e repassa-o para outra função que analisa o tópico e a mensagem para tratamento correto dos dados e centralização no SBC. Na função _evaluateRecData(char *, char*)_ os dados são avaliados e ajustados para a correta manipulação. Nas respostas do status do NodeMCU, é apenas escrito na IHM Display e enviado para um outro tópico o resultado formatado de acordo com a resposta. Nestes casos a resposta recebida é constituída de 2 caractéres, que denotam o tipo de status recebido, onde '1F' indica que a NodeMCU está com algum problema e '00 ' indica que a situação está ok.

No caso do sensor analógico, a mensagem recebida é composta por 2 caractéres que denotam o tipo de sensor e os caractéres restantes da string são o valor medido nele (Exemplo: 015, ou então, 01789). É criada uma nova variável que recebe uma substring da mensagem total de modo que receba apenas o valor de medição, é convertido em outra variável para tipo inteiro usando a função _atoi(char *)_. O valor recebido é exibido no IHM Display e enviado para ser guardado no histórico.

Semelhantemente ocorre com os sensores digitais, a sutileza presente para eles se trata do formato de mensagem recebido. Neste modelo, a mensagem recebida (Exemplo: 02D3987) é composta pelos 2 caractéres de tipo de sensor (Ex.: 02), os próximos 2 caractéres (D3) são indicativos de qual sensor digital foi medido e o restante da string o valor da medição (987). O tratamento também é parecido, onde duas variáveis são criadas: 1 para o nome do sensor e outra para o valor. Seguindo o mesmo procedimento, o valor é convertido para inteiro e repassado para uma outra função que guarda a medição no histórico.

###### Passo 6
Este passo somente é realizado para mensagens recebidas sobre os sensores analógicos e digitais. Neste trecho do programa um índice temporário foi criado junto com uma string que receberá, em notação JSON, os dados escritos do sensor. Esse índice temporário é atribuído o índice pré-incrementado de última modificação do sensor. Deste modo, o campo *last_modified* está sempre atualizado em todos os casos. Caso o campo *last_modified* seja maior ou igual a 9, ele é reiniciado como -1.

Assim, o valor e o timestamp recebidos sempre ocuparão a mesma posição em seus vetores adequados (De modo que caso queira acessar historico[3], pode-se, com o índice igual, acessar o timestamp correspondente como timestamp[3]). Outra vantagem de usar o *last_modified* é que os valores dos histórico são constantemente atualizados quando o limite de 10 medições é atingido, assim, quanto menor o índice no vetor a mais tempo ele está presente e deve ser o primeiro a ser atualizado.
Após o histórico e o timestamp estarem corretamente atualizados, a função *createJson(int)* é chamada e escreve os valores medidos e tempos numa string de modo a estar formatado em JSON.

###### Passo 7
Continuando o passo 6, com a criação do JSON e dados centralizados na SBC, resta apenas transmitir os dados históricos para a IHM Web. No tópico "SENSORS_HISTORY/nome-do-sensor" a mensagem é publicada e a IHM Web, inscrita nele, recebe a string JSON que a decodifica e transforma em objeto Javascript. Portanto, com a escrita correta do histórico e string formatada, os dados são exibidos em gráficos correspondentes a cada sensor. É importante ressaltar que existe, para cada sensor, um tópico que tem escrito o histórico respectivo a ele. Foi implementado desta forma pois é interessante que se separe os dados individualmente para não existir conflitos entre sensores e evitar incoerência entre os dispositivos e a comunicação MQTT.

O código do SBC está definido <a href="https://github.com/ian-zaque/pbl_SD_3/blob/main/mqtt.c">aqui</a>. Ele é responsável pelo funcionamento e centralização do programa.

<details>
<summary>Funções para o SBC</summary>
Foram criadas algumas funções para modularização e consistência do código. Elas estão listadas e descritas abaixo:

###### void publish(MQTTClient client, char* topic, char* payload);
Esta função é responsável pelo envio das mensagens para um tópico passado como parâmetro. Pode ser usada em qualquer lugar do código. Valendo-se da biblioteca ```MQTTClient.h```, foi possível configurar a troca de mensagens e o cliente MQTT.

<details>
<summary>Parâmetros</summary>

1. MQTTClient client
    - Cliente de conexão MQTT, definido globalmente antes da função ```main()```.
2. char * topic
    - String com nome do tópico onde será escrita a mensagem.
3. char * payload
    - String da mensagem a ser transmitida pela conexão MQTT
</details>

<hr>

###### int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
Esta função está em execução a todo tempo desde que o MQTT tenha sido configurado, iniciado e o cliente esteja conectado ao broker. Esta execução contínua é feita ao passar a função ```on_message``` como 4º parágrafo da função ```MQTTClient_setCallbacks()```. No corpo da função o payload é persistido numa variável local e em seguida é chamada a função de avaliação de tópico e mensagem recebidos. Os passos seguintes são próprios da biblioteca MQTT e liberam da memória as variáveis de mensagem e tópico dos parâmetros. O retorno da função é 1.

<details>
<summary>Parâmetros</summary>

1. void *context
    - Ponteiro nulo para que qualquer tipo de dado possa ser passado como dados de contexto.
2. char * topicName
    - String com nome do tópico onde será escrita a mensagem.
3. int topicLen
    - Valor inteiro do comprimento do tópico.
4. MQTTClient_message * message
    - Ponteiro de tipo MQTTClient_message com informações da mensagem recebida pelo cliente.
</details>

<hr>

###### void reconnect(void *context, char *cause);
Função de tipo de retorno vazio que possibilita a reconexão ao broker em caso de queda/desconfiguração. Ela é acionada quando não é detectado mais comunicação com o broker. O corpo da função é semelhante ao passo 2 da Comunicação SBC - ESP. Em caso de insucesso na reconexão o programa é encerrado e retorna -1, porém, caso seja possível reconectar-se, o cliente é inscrito novamente nos tópicos *NODEMCU_RECEIVE* e *IHM_TIMECHANGE*.

<details>
<summary>Parâmetros</summary>

1. void *context
    - Ponteiro nulo para que qualquer tipo de dado possa ser passado como dados de contexto.
2. char * cause
    - String com possível motivo da causa da queda de conexão.
</details>

<hr>

###### void evaluateRecData(char *topicName, char *payload);
Função de tipo de retorno vazio que avalia e verifica as mensagens recebidas e seus respectivos tópicos. Como o cliente SBC está subscrito em 2 tópicos, ele irá receber mensagens nos dois em dado momento. 

- Caso receba dados no tópico *IHM_TIMECHANGE*, o dado é convertido em inteiro e multiplicado por 1000. Assim o dado irá ser reutilizado de forma já tratada quando for publicado para o NodeMCU no tópico *NODEMCU_TIMECHANGE*. 

- Caso receba dados no tópico *NODEMCU_RECEIVE*, o dado é esmiussado por caractéres. Se o payload for igual a:
    - **1F**, a resposta enviada pelo NodeMCU foi de que houve algum erro no momento de análise. É escrito na IHM Display o status do NodeMCU e publicado no tópico *STATUS_NODEMCU*.

    -**00**, o NodeMCU tem status de conexão ok e opera normalmente. De mesmo modo que antes, é escrito na IHM Display o status do NodeMCU e publicado no tópico *STATUS_NODEMCU* que está conectado.

    - **01**, é correto afirmar que foi enviado pelo NodeMCU o valor medido do sensor analógico. A mensagem recebida possui formato "01" concatenado ao valor capturado pelo sensor. O valor é resgatado e splitado da mensagem original através da função ``` substring(char *, int, int);```, convertido em inteiro e repassado para atualizar o histórico do sensor analógico. É escrito também no IHM Display o nome do sensor, A0, e o valor medido.

    - **02**, é correto afirmar que foi enviado pelo NodeMCU o valor medido por algum sensor digital. A mensagem do sensor digital possui formato "02" concatenado ao nome do sensor digital concatenado ao valor capturado pelo sensor (Por exemplo, para o sensor D2 que enviou o valor 1024, a mensagem original é: 02D21024). O nome do sensor e o valor medido são resgatados da mensagem original através da função ``` substring(char *, int, int);``` (chamando-a duas vezes, uma para cada campo e passando índices diferentes de início e fim). O valor do sensor éc onvertido em inteiro e repassado para atualizar o histórico do respectivo sensor digital. É escrito também no IHM Display o nome do sensor e o valor medido.

    - nenhum outro valor, entende-se que houve um erro desconhecido no NodeMCU e é exibido no IHM Display que o status de conexão não está correto.

<details>
<summary>Parâmetros</summary>

1. void *topicName
    - String com nome do tópico onde foi escrita a mensagem.
2. char * payload
    - String da mensagem que foi transmitida pelo NodeMCU.
</details>

<hr>

###### char *substring(char *src, int start, int end);
Função de tipo de retorno de vetor de caractéres (string) que realiza um corte na string _src_ a partir de dois índices passados como parâmetro. Retorna uma nova string iniciada no índice _start_ do vetor e terminando em _end_. No corpo da função, é visto o tamanho da string a ser criada e reserva-se, com o método _malloc_, espaço de memória para a string de destino. Usando do método _strncpy_ da biblioteca _string.h_, a nova string é copiada para o destino verificando os índices de começo e final. Por fim a string de destino é retornada.

<details>
<summary>Parâmetros</summary>

1. char *src
    - String a ser cortada.
2. int start
    - Índice de início da nova string.
3. int end
    - Índice de final da nova string.
</details>

<hr>

###### void updateHistory(char * sensor, int newValue);
Função  de tipo de retorno vazio criada para atualizar ou criar os valores dos vetores de histórico e timestamp dos sensores. No início do corpo da função, é criado um inteiro _idx_ inicializado em 0 (será usado para reutilizar mais facilmente o campo *last_modified* dos sensores). Em seguida instancia-se a variável *now*, que guardará o valor em milissegundos em que foi recebida a medição para o sensor. Também são criadas outras 2 variáveis locais: *json* que será a string em formato JSON com os vetores de histórico e timestamp; e *sensorTopic* que reterá o nome do tópico respectivo ao sensor que está sendo atualizado (tópico terá o nome *SENSORS_HISTORY/* concatenado ao nome do sensor, ex.: *SENSORS_HISTORY/A0*. Este exemplo, o tópico será escrito quando atualizado os valores medidos e o horário para o sensor analógico A0).

É feita a verificação se o sensor existe nos sensores conhecidos pela SBC e só então o histórico será atualizado. O índice _idx_ recebe o *last_modified* pré-incrementado do sensor e verifica-se qual o valor atual. Caso seja maior ou igual 9, quer dizer que já está ou passou do número possíveis de medições. Em seguida o valor do último sensor modificado é atualizado junto com o timestamp. Por fim o valor é copiado para a variável *json* depois dos campos serem formatados e postos em JSON. Esta formatação é realizada na função ``` createJson(int); ``` que retorna uma string. Caso o sensor não exista/não seja reconhecido apenas alerta-se que houve um sensor desconhecido, sem atualizar nada em histórico.

É importante ressaltar que quando o valor qualquer do vetor de timestamp de um sensor for 0, não houve medição para esta posição ainda. Isto vale para todos os sensores, todas as posições e independe do valor da medição. Em resumo, caso o valor do timestamp da posição *n* for 0, o sensor não foi medido *n* vezes ainda. Caso haja valor na posição *n* no timestamp e valor 0 para histórico, quer dizer que o valor medido do sensor foi 0.

![Imagem do início da função updateHistory e a verificação para o primeiro sensor, o sensor analógico A0](./images/updateHistory.png)

<details>
<summary>Parâmetros</summary>

1. char *sensor
    - String do nome do sensor.
2. int newValue
    - Novo valor a ser posto no histórico de medições.
</details>

<hr>

###### char *createJson(int index);
Função de tipo de retorno vetor de caractér (string) de criação de string formatada em JSON. Recupera os vetores de histórico e timestamp de um sensor, concatenando e manipulando strings para simular um JSON real criado com Javascript. 
A string criada possui formato como no exemplo: *"{historico: [0,0,0,0,0,0,0,0,0], timestamp: [0,0,0,0,0,0,0,0,0] }"*. No corpo da função, aloca-se memória para a string com tamanho suficiente para capturar dados muito grandes caso existam. 

Em seguida são postos dois laços de repetição em sequência: o primeiro concatena o campo de histórico, os caracteres que formam o JSON (como chaves, dois pontos e etc) e os 10 valores medidos até o momento. Após isso, os mesmos passos são feitos para o vetor timestamp, de modo que o resultado final seja como o exemplo citado acima. Exibe-se na IHM Terminal a string resultante da manipulação e retorna-se o valor para onde a função foi chamada.

<details>
<summary>Parâmetros</summary>

1. int index
    - Índice do sensor em *array_registros*.
</details>

<hr>

###### void initArrayRegistros();
Função de tipo de retorno vazio que inicializa a variável global *array_registros*. Seta os 10 nomes de sensores e em dois laços de repetição inicializa as posições dos vetores de histórico e timestamp em 0 em ambos. Também inicializa o campo *last_modified* em -1, para que quando for pré-incrementado o primeito alvo de modificação seja o índice 0.

<hr>

###### void initDisplay();
Função de tipo de retorno vazio que inicializa o LCD Display. Utiliza as bibliotecas *wiringPi.h*, *wiringPiDev.h* e *lcd.h* para manipular os pinos GPIO e deixá-los disponíveis para escrita no display. A variável global *display_lcd* recebe um valor inteiro que simboliza um endereço de memória para ser identificado e junto ao processador gerir processos de Entrada e saída.

<hr>

###### void write_textLCD(char *linha1, char *linha2);
Função de tipo de retorno vazio que permite a escrita de strings em duas linhas do LCD Display. Utiliza a variável global *display_lcd* para determinar o Display e ser capaz de escrever nele. O limite de caractéres por linha é 15.

<details>
<summary>Parâmetros</summary>

1. char *linha1
    - String a ser escrita na primeira linha do LCD Display.
2. char *linha2
    - String a ser escrita na segunda linha do LCD Display.
</details>

<hr>

</details>


<h2>Comunicação SBC - Interface Web</h2>
<h4>API</h4>
Como decisão de projeto foi desenvolvido uma api, na linguagem Javascript, .para se comunicar com a SBC e a interface web para visualização  dos dados graficamente. E está estruturado nas seguintes ordens: 1- Comunicação Cliente MQTT,  2- Comunicação servidor HTTP e 3- Interface web HTML
<h6>Passo 1</h6>
A API trabalha com dois protocolos de comunicação  MQTT e o HTTP. Internamente é executado um cliente MQTT (figura 5) para comunicação direta com a SBC, este cliente recebe todos os dados formatados que a SBC tratou, esses dados são referentes aos valores das últimas dez medições dos sensores. Os dados chegam em formato de string JSON o qual é transformado em um objeto JSON(figura 6) e armazenado em um arquivo .JSON (fiigura 7) que temos como base de dados. Para que isso fosse possível a API teve que  se cadastrar em cada tópico relacionado a uma medição do sistema (figura 8).

![Imagem da Configuracao MQTT](./images/configuracao_mqtt.png)
Figura 5: Configuração Conexão MQTT

![Imagem do recebimento de dados](./images/recebimento_msg.png)
Figura 6: Conversão da Mensagem em objeto JSON

![Imagem do armazenamento dos dados](./images/escrita_dados_json.png)
Figura 7: Armazenamento dos dados recebidos

![Imagem dos inscrição dos tópicos](./images/definicao_inscricao_topicos.png)
Figura 8: Inscrição nos tópicos


<h6>Passo 2</h2>

API executa um servidor HTTP, na porta 3000, o qual fica esperando a comunicação da interface web via requisições HTTP. 
O servidor dispõe de três rotas de acesso “/comando, /situacaoNode, /sensores/:sensor”:
<ul>
    <li>“/comando”: Rota responsável por enviar um comando de ação via protocolo MQTT que será executado na ESP8266. Para esse caso, é enviado a informação de alteração to tempo de medição dos sensores do ESP8266</li>
    <li>“/situacaoNode”: essa rota retorna apenas qual o status da ESP8266 se está conectado ou desconectado. Se for conectado sabemos que o dispositivo está funcionando corretamente, caso contrário o dispositivo não está funcionando corretamente. </li>
        <li>“/sensores/:sensor”:  Essa rota recebe como parâmetro o nome do sensor que queremos visualizar os dados e como resposta é retornado um objeto JSON que remete aos dados das medições mais atualizadas dos sensores naquele momento. Para a execução dessa tarefa é feita a leitura do arquivo JSON(figura 9) no momento que é identificado de qual sensor queremos visualizar os dados. 
 </li>    
</ul>

![Imagem da leitura do arquivo JSON](./images/rotas_http.png)
Figura 9: Leitura da base de Dados (arquivo JSON)
    
   <h6>Passo 3  - Interface Web</h6>
   É uma simples página HTML (figura 10) que permite o usuário interagir com o sistema. Ela se comunica diretamente com a API via protocolo HTTP(figura 11). A interface dispõe de nove botões que fazem referência aos sensores disponíveis na ESP8266, através desses botões o usuário pode visualizar um gráfico de medições referente aos dados medidos pelo sensor escolhido. O usuário pode alterar o tempo em que a ESP8266 realiza as medições dos dados dos sensores. Nos campos de input, o primeiro é possível solicitar a alteração do tempo de medição dos sensores da ESP8266 e o segundo altera o tempo em que é atualizada os gráficos da interface web. (figura 10)
A Interface Web se comunica apenas com a API do sistema, não conhecendo os outros elementos que compõem o  sistema como a SBC, a ESP8266, o Broker. 

![Imagem da Inteface](./images/interface_web.png)
Figura 10: Interface HTML

![Imagem da conexao HTTP](./images/requisicao_axios.png)
Figura 11: Modelo da requisição de um dado para o servidor

<h2>ESP8266</h2>

A configuração com a ESP8266 foi realizada via WiFi, nela foi configurado um cliente MQTT para realizar toda a transferência de dados. Seguindo o mesmo modelo do problema anterior, a ESP8266  espera de um código de comando para realizar uma determinada tarefa e retornar a resposta. 
Neste projeto a ESP esperava um comando de 2 dígitos, que era convertido em string, o primeiro dígito se referia a ação que deveria ser realizada e o segundo dígito a algum sensor caso houvesse. Como resposta a ESP retornava uma mensagem no seguinte formato: 

```
Mensagem de Resposta:
    código_resposta+node_sensor+valor_sensor
```
Exceto quando é apenas solicitado a situação da ESP, o qual ela responderá apenas com o código de resposta. A figura 12 mostra como é feita essa verificação e a mensagem de resposta que é montada quando o cliente mqtt publica os dados.

![Imagem resposta node](./images/response_node_mqtt.png)
Figura 12: Publicação da mensagem do ESP8266

<h1>Conclusão e Considerações Finais</h1>

O sistema desenvolvido não atende todos os requisitos requeridos no Problema 3, ficando em falta a interação do usuário com a interface local, a visualização do histórico e menu no display LDC. Foi acrescentada a possibilidade de se alterar o intervalo de tempo das medições através da interface web. 

<h1>Referências</h1>

Graça, Pedro Cannale. Sistema de aquisição de dados utilizando o módulo ESP8266 NodeMCU / Pedro Cannale Graça - Guaratinguetá, 2017.

COSTA NETO, J. V. da; BARROS FILHO, E. M. de .; SANTANA, J. R. . Single-Board Computers in Education: A systematic literature review.

Torres. Andrei B.B., Rocha. Atslands R., Souza. José Neuman. Análise de Desempenho de Brokers MQTT em Sistema de Baixo Custo. XXXVI Congresso da Sociedade Brasileira de Computação. - 15º Workshop em Desempenho de Sistemas Computacionais e de Comunicação. 


