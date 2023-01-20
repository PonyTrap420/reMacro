const mongoose = require('mongoose');
const { toJSON } = require('./plugins');

const downloadsScheme = mongoose.Schema(
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
  },
  {
    timestamps: true,
  }
);

// add plugin that converts mongoose to json
downloadsScheme.plugin(toJSON);

downloadsScheme.statics.AlreadyDownloaded = async function (user, macro) {
    const download = await this.findOne({ user: user, macro: macro });
    return !!download;
};
  

/**
 * @typedef Downloads
 */
const Downloads = mongoose.model('Downloads', downloadsScheme);

module.exports = Downloads;
