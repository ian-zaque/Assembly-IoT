let dados = [];
let stop;
let time;
var chartGraph;
axios.get("http://localhost:3000/situacaoNode").then(response=>{
  let situacao = document.getElementById("situacaoNode");
  situacao.innerHTML = response.data;
  console.log(response)
}).catch(e=>{
  alert("Erro 500: Servidor desconectado")
})
function listaHistorico() {
  console.log(dados);
}

const padTo2Digits = (num) => {
  return num.toString().padStart(2, "0");
};

//Obtem o valor de um sensor específico
function lista_sensor(path) {
  var ctx = document.getElementById("line-chart");
  let chartStatus = Chart.getChart("line-chart"); // <canvas> id

  let name_sensor = "Sensor " + path;
  if (chartStatus != undefined) {
    console.log("canvas used");
    chartStatus.destroy();
  }
  chartGraph = null;
  speedReq();

  //let historico;
  //let timestamps;
  if (!time) {
    stop = setInterval(() => {
      axios
        .get("http://localhost:3000/sensores/" + path)
        .then((response) => {
          dados = response.data;
          const json_values = dados[0];
          const{historico,timestamps} = json_values
          //historico = json_values.historico;
          //.log(typeof(historico))
          
          //let historico_filter = historico.filter(element=> element!=0)
          let times_config = formataData(timestamps);
          
        
          //console.log("TImes: ", historico_filter);
          if (chartGraph != null) {
            chartGraph.destroy();
          }
          chartGraph = new Chart(ctx, {
            type: "line",
            data: {
              labels: times_config,
              datasets: [
                {
                  label: name_sensor,
                  backgroundColor: "rgba(0,0,0,1.0)",
                  borderColor: "rgba(0,0,0,0.5)",
                  data: historico,
                },
              ],
            },
            options: {
              scales: {
                y: {
                  beginAtZero: true,
                },
              },
              plugins: {
                title: {
                  display: true,
                  text: "Sensores",
                  padding: {
                    top: 10,
                    bottom: 30,
                  },
                },
              },
            },
          });
        })
        .catch((e) => {
          alert("Error 500: Servidor desconectado")
        });
    }, 2000);
  } else {
    stop = setInterval(() => {
      axios
        .get("http://localhost:3000/sensores/" + path)
        .then((response) => {
          dados = response.data;
          const json_values = dados[0];
          const { historico,timestamps } = json_values;
          let conft = formataData(timestamps);
          console.log(conft)
          if (chartGraph != null) {
            chartGraph.destroy();
          }
          chartGraph = new Chart(ctx, {
            type: "line",
            data: {
              labels: conft,
              datasets: [
                {
                  label: name_sensor,
                  backgroundColor: "rgba(0,0,0,1.0)",
                  borderColor: "rgba(0,0,0,0.5)",
                  data: historico,
                },
              ],
            },
            options: {
              scales: {
                y: {
                  beginAtZero: true,
                },
              },
              plugins: {
                title: {
                  display: true,
                  text: "Sensores",
                  padding: {
                    top: 10,
                    bottom: 30,
                  },
                },
              },
            },
          });
        })
        .catch((e) => {
          alert("Error 500: Servidor desconectado")
        });
    }, time);
  }
}

//Altera tempo de atualização dos dados
function speedReq() {
  if (stop) {
    clearInterval(stop);
    console.log(stop);
    var input = document.querySelector("#time");
    var value = input.value;
    time = value;
    chartGraph = null;
  }
}
function enviaComando() {
  var input = document.querySelector("#comando");
  var value = input.value;
  axios.post("http://localhost:3000/comando", { comando: value });
  console.log(value);
}

// Desenha Grafico
function formataData(timestamps) {
  const value_times = JSON.parse(timestamps);
  let time = [];

  value_times.forEach((element) => {
    let data = new Date(element);
    //console.log(data);
    let hours = data.getHours();
    let minutes = data.getMinutes();
    let seconds = data.getSeconds();

    const time_format = `${padTo2Digits(hours)}:${padTo2Digits(
      minutes
    )}:${padTo2Digits(seconds)}`;
    time.push(time_format);
  });
  return time;
}
