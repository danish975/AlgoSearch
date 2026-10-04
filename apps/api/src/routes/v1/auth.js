"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.authRouter = void 0;
const express_1 = require("express");
const auth_1 = require("../../controllers/auth");
const auth_2 = require("../../middleware/auth");
exports.authRouter = (0, express_1.Router)();
exports.authRouter.post('/register', auth_1.authController.register);
exports.authRouter.post('/login', auth_1.authController.login);
exports.authRouter.get('/me', auth_2.authenticate, auth_1.authController.me);
//# sourceMappingURL=auth.js.map