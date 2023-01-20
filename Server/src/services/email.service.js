const nodemailer = require('nodemailer');
const config = require('../config/config');
const logger = require('../config/logger');

const transport = nodemailer.createTransport(config.email.smtp);
  transport
    .verify()
    .then(() => logger.info('Connected to email server'))
    .catch(() => logger.warn('Unable to connect to email server.'));

const sendEmail = async (to, subject, text) => {
  const msg = { from: config.email.from, to, subject, text };
  await transport.sendMail(msg);
};

const sendResetPasswordEmail = async (to, token) => {
  const subject = 'Reset password';
  const text = `
To reset your password, Enter this secret key to proper field in the app.
Secret Key: ${token}
If you did not request any password resets, then ignore this email.`;
  await sendEmail(to, subject, text);
};

const sendVerificationEmail = async (to, token) => {
  const subject = 'Email Verification';
  const text = `
  To verify your email, Enter this secret key to proper field in the app.
  Secret Key: ${token}
  If you did not create an account, then ignore this email.`;
    await sendEmail(to, subject, text);
};

module.exports = {
  transport,
  sendEmail,
  sendResetPasswordEmail,
  sendVerificationEmail,
};
