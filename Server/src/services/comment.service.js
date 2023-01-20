const { Comment } = require('../models');

const addComment = async (user, macro, content) => {
    const commentDoc = await Comment.create({user, macro, content});
    return commentDoc;
};

const getComments = async (macro, page) => {
    return Comment.find({macro}).skip(10*page).limit(10);
  };

module.exports = {
    addComment,
    getComments
};
