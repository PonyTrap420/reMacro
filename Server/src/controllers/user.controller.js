const httpStatus = require('http-status');
const ApiError = require('../utils/ApiError');
const catchAsync = require('../utils/catchAsync');
const { userService, avatarService, macroService, commentService} = require('../services');

const uploadAvatar = catchAsync(async (req, res) =>{
  if(req.file == undefined){
      throw new ApiError(httpStatus.FORBIDDEN, 'Unsupported file type');
  }
  await avatarService.updateUserAvatar(req.user, req.file);
  res.status(httpStatus.NO_CONTENT).send();
});

const uploadMacro = catchAsync(async (req, res) =>{
    const macro = await macroService.createMacro(req.user, req.body);
    res.send(macro);
});

const addComment = catchAsync(async (req, res) =>{
  await commentService.addComment(req.user, req.query.macro, req.body.content);
  res.status(httpStatus.NO_CONTENT).send();
});

const getComments = catchAsync(async (req, res) => {
  const comments = await commentService.getComments(req.query.macro, req.query.page);
  res.send(comments);
});

const downloadMacro = catchAsync(async (req, res) =>{
  await macroService.downloadMacro(req.user, req.query.macro);
  res.status(httpStatus.NO_CONTENT).send();
});

const getMacros = catchAsync(async (req, res) =>{
  const macros = await macroService.getMacros(req.query.page);
  res.send(macros);
});

const getUserMacros = catchAsync(async (req, res) =>{
  const macros = await macroService.getUserMacros(req.params.user);
  res.send(macros);
});

const searchMacro = catchAsync(async (req, res) => {
  const macros = await macroService.searchMacro(req.query.name, req.query.page);
  res.send(macros);
});

const getUser = catchAsync(async (req, res) => {
  var user = await userService.getUserById(req.params.userId);
  if (!user) {
    throw new ApiError(httpStatus.NOT_FOUND, 'User not found');
  }
  user.email = undefined;
  user.isEmailVerified = undefined;
  res.send(user);
});

const updateUser = catchAsync(async (req, res) => {
  const user = await userService.updateUserById(req.params.userId, req.body);
  res.send(user);
});

const deleteUser = catchAsync(async (req, res) => {
  await userService.deleteUserById(req.params.userId);
  res.status(httpStatus.NO_CONTENT).send();
});

module.exports = {
  uploadAvatar,
  uploadMacro,
  addComment,
  getComments,
  downloadMacro,
  getMacros,
  getUserMacros,
  searchMacro,
  getUser,
  updateUser,
  deleteUser,
};
