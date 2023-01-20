const mongoose = require('mongoose');
const { toJSON } = require('./plugins');

const macroSchema = mongoose.Schema(
  {
    name: {
      type: String,
      required: true,
      trim: true
    },
    description:{
      type: String, 
      required: true,
      trim: true
    },
    user: {
      type: mongoose.SchemaTypes.ObjectId,
      ref: 'User',
      required: true,
    },
    macro: {
      type: String,
      required: true,
    },
    downloads: {
      type: Number,
      default: 0,
    },
  },
  {
    timestamps: true,
  }
);

// add plugin that converts mongoose to json
macroSchema.plugin(toJSON);

macroSchema.statics.SameMacro = async function (name, userId) {
  const macro = await this.findOne({ name: name, user: userId });
  return !!macro;
};

/**
 * @typedef Macro
 */
const Macro = mongoose.model('Macro', macroSchema);

module.exports = Macro;
