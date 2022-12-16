const { readFile, writeFile } = require("fs/promises");

const ReadFile =async(path)=>{
    return JSON.parse(await readFile(path));
}
const WriteFile = async(path,message)=>{

    let currentFile = await ReadFile(path);
    currentFile=[];
    const {historico,timestamps} = message;
    let data = { historico,timestamps };
    currentFile.push(data);
    writeFile(path, JSON.stringify(currentFile));
}

module.exports = {ReadFile,WriteFile};