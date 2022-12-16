const { readFile, writeFile } = require("fs/promises");

const historicos = '{ "historico": "[270,236,220,200,289,275,100,23]","timestamps": " [410811,412551,412580,412702,413003,413102,413122,413202]"}';

const teste = () => {
  const value = JSON.stringify(historicos);
  const array = JSON.parse(value)
  return JSON.parse(array);
};
// Ler Base de dados (arquivo Json)
const ReadFile = async () => {
  return JSON.parse(await readFile("data_base.json"));
};

const padTo2Digits = (num) => {
  return num.toString().padStart(2, "0");
};
const formatData = () => {
  let array = teste();
  const { historico, timestamps } = array;
  const value_hist = JSON.parse(historico);
  const value_times = JSON.parse(timestamps);

  let time = [];

  value_times.forEach((element) => {
    let data = new Date(element * 1000);
    let hours = data.getHours();
    let minutes = data.getMinutes();
    let seconds = data.getSeconds();

    const time_format = `${padTo2Digits(hours)}:${padTo2Digits(
      minutes
    )}:${padTo2Digits(seconds)}`;
    time.push(time_format);
  });
  return time;
};

const t = teste();
// const a = JSON.parse(t)

// const {historico,timestamps} = a
// const value_hist = JSON.parse(historico);
console.log(formatData())
