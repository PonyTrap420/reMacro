const httpStatus = require('http-status');
const { Macro } = require('../models');
const { Downloads } = require('../models')
const ApiError = require('../utils/ApiError');

const createMacro = async (user, MacroBody) => {
  if (await Macro.SameMacro(MacroBody.name, user)) {
    throw new ApiError(httpStatus.BAD_REQUEST, 'Cant have two macros with the same');
  }
  const macroDoc = await Macro.create({
    name: MacroBody.name,
    description: MacroBody.description,
    user,
    macro: MacroBody.macro
  });
  return macroDoc;
};

const getMacros = async (page) => {
  return Macro.find().sort ({downloads:-1}).skip(10*page).limit(10);
};

const getUserMacros = async(user) => {
  return Macro.find({user});
};

const searchMacro = async(name, page) =>{
  return Macro.find({ "name": { $regex: name, $options: "i" } }).sort ({downloads:-1}).skip(10*page).limit(10);
};

const downloadMacro = async(user, macro) => {
  if (await Downloads.AlreadyDownloaded(user, macro)) {
    throw new ApiError(httpStatus.BAD_REQUEST, 'Already Downloaded');
  }
  const downloadDoc = await Downloads.create({
    user: user,
    macro: macro
  });

  await IncrementMacroDownloads(macro);

  return downloadDoc;
};

const getMacroById = async (id) => {
  return Macro.findById(id);
};

const IncrementMacroDownloads = async (MacroId) => {
  const Macro = await getMacroById(MacroId);
  if (!Macro) {
    throw new ApiError(httpStatus.NOT_FOUND, 'Macro not found');
  }
  Object.assign(Macro, {downloads: Macro.downloads + 1});
  await Macro.save();
  return Macro;
};

const updateMacroById = async (MacroId, updateBody) => {
  const Macro = await getMacroById(MacroId);
  if (!Macro) {
    throw new ApiError(httpStatus.NOT_FOUND, 'Macro not found');
  }
  Object.assign(Macro, updateBody);
  await Macro.save();
  return Macro;
};

const deleteMacroById = async (MacroId) => {
  const Macro = await getMacroById(MacroId);
  if (!Macro) {
    throw new ApiError(httpStatus.NOT_FOUND, 'Macro not found');
  }
  await Macro.remove();
  return Macro;
};

module.exports = {
  createMacro,
  getMacros,
  downloadMacro,
  getUserMacros,
  searchMacro,
  getMacroById,
  updateMacroById,
  deleteMacroById,
};
