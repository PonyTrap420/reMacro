const { User } = require('../models');
const axios = require('axios');

async function getSvg() {
    const url = "https://avatars.dicebear.com/api/identicon/"+Date.now()+".svg";
    let response = await axios.get(url);
    return response.data;
}

const updateUserAvatar = async (user, file) => {
    User.findByIdAndUpdate(user.id, {avatar: file.filename}, ()=>{})
};

const generateAvatar = async(user) => {
    const svg = await getSvg();

    return User.findByIdAndUpdate(user.id, {avatar: svg}, ()=>{})
}

module.exports = {
    updateUserAvatar, 
    generateAvatar
}