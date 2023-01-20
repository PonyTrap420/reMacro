const Joi = require('joi');
const { password, objectId } = require('./custom.validation');

const uploadAvatar = {
    body: Joi.any()
};

const uploadMacro = {
  body: Joi.object().keys({
    name: Joi.string().required(),
    description: Joi.string().required(),
    macro: Joi.string().required(),
  }),
};

const addComment = {
  query: Joi.object().keys({
    macro: Joi.string().required().custom(objectId),
  }),
  body: Joi.object().keys({
    content: Joi.string().required()
  }),
};

const getComments = {
  query: Joi.object().keys({
    macro: Joi.string().required().custom(objectId),
    page: Joi.string().required()
  }),
};

const getMacros = {
  query: Joi.object().keys({
    page: Joi.string().required()
  }),
};

const getUserMacros = {
  params: Joi.object().keys({
    user: Joi.string().custom(objectId),
  }),
};

const searchMacro = {
  query: Joi.object().keys({
    name: Joi.string().required(),
    page: Joi.string().required()
  }),
}

const downloadMacro = {
  query: Joi.object().keys({
    macro: Joi.string().custom(objectId),
  }),
};

const getUser = {
  params: Joi.object().keys({
    userId: Joi.string().custom(objectId),
  }),
};

const updateUser = {
  params: Joi.object().keys({
    userId: Joi.required().custom(objectId),
  }),
  body: Joi.object()
    .keys({
      email: Joi.string().email(),
      password: Joi.string().custom(password),
      name: Joi.string(),
    })
    .min(1),
};

const deleteUser = {
  params: Joi.object().keys({
    userId: Joi.string().custom(objectId),
  }),
};

module.exports = {
  uploadAvatar,
  uploadMacro,
  addComment,
  getComments,
  getMacros,
  searchMacro,
  getUserMacros,
  downloadMacro,
  getUser,
  updateUser,
  deleteUser,
};
