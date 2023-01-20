const multer = require('multer');
const fs = require('fs')

const removeLast = (req) => {
    if(! req.user.avatar.startsWith("<svg xmlns:dc=")){
        const path = "uploads/"+req.user.avatar;

        fs.unlink(path, (err) => {
            if (err) {
                console.error(err)
                return
            }
        })
    }
}

const storage = multer.diskStorage({
    destination: (req, file, cb) =>{
        cb(null, "uploads/");
    },
    filename: (req, file, cb) =>{
        cb(null, Date.now() + "-" + file.originalname);
    }
});

const fileFilter = (req, file, cb) =>{
    if(file.mimetype === "image/jpeg" || file.mimetype === "image/png" || file.mimetype === "image/jpg"){
        removeLast(req);
        cb(null, true);
    }
    cb(null, false);
};

const upload = multer({
    storage,
    limits:{
        fileSize: 1024*1024*8
    },
    fileFilter,
});

module.exports = upload;