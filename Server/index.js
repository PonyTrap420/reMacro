const express = require('express')
const fs = require('fs')
const https = require('https')
const path = require('path')

const app = express()
const PORT = 8443;

app.use(express.json())

const sslServer = https.createServer({
    key: fs.readFileSync(path.join(__dirname, 'cert', 'key.pem')),
    cert: fs.readFileSync(path.join(__dirname, 'cert', 'cert.pem'))
},app)

sslServer.listen(PORT, () => console.log("Server running"))

app.get("/", (req, res) =>{
    console.log(req)
    res.status(200).send({
        nice2: req.body
    })    
})


app.post("/remove-macro/:id", (req, res) => {
    const {id} = req.params;
    const {reason} = req.body;

    if(!reason){
        return res.status(418).send({msg:"No reason provided!"})
    }
    else if(!parseInt(id)){
        return res.status(418).send({msg:"No reason provided!"})
    }

    res.send(
        {
            msg: `Macro ${id} has been removed due to ${reason}`
        }
    )
})
