const mongoose = require('mongoose');
const { toJSON } = require('./plugins');

const commentScheme = mongoose.Schema(
  {
    user: {
      type: mongoose.SchemaTypes.ObjectId,
      ref: 'User',
      required: true
    },
    macro: {
      type: mongoose.SchemaTypes.ObjectId,
      ref: 'Macro',
      required: true
    },
    content: {
      type: String,
      required: true,
      trim: true
    }
  },
  {
    timestamps: true,
  }
);

// add plugin that converts mongoose to json
commentScheme.plugin(toJSON);

/**
 * @typedef Comment
 */
const Comment = mongoose.model('Comment', commentScheme);

module.exports = Comment;
