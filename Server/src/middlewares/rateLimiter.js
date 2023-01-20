const rateLimit = require('express-rate-limit');
const ApiError = require('../utils/ApiError');
const httpStatus = require('http-status');


const limitReached = (req, res) => {
  throw new ApiError(httpStatus.TOO_MANY_REQUESTS, 'too many requests');
 }

const authLimiter = rateLimit({
  windowMs: 15 * 60 * 1000,
  max: 20,
  skipSuccessfulRequests: true,
});

const emailLimiter = rateLimit({
  windowMs: 60000 * 2,
  max: 1,
  handler: limitReached,
})

const commentLimiter = rateLimit({
  windowMs: 60000 * 5,
  max: 5,
  handler: limitReached,
})

module.exports = {
  authLimiter,
  emailLimiter,
  commentLimiter
};
