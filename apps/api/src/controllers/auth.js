"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.authController = void 0;
const express_1 = require("express");
const jsonwebtoken_1 = __importDefault(require("jsonwebtoken"));
const user_1 = require("../models/user");
const config_1 = require("../config");
const zod_1 = require("zod");
const registerSchema = zod_1.z.object({
    email: zod_1.z.string().email(),
    password: zod_1.z.string().min(6), // using simple string since we'll hash it, omitting actual hash for simplicity in this MVP
    name: zod_1.z.string().min(2),
});
const loginSchema = zod_1.z.object({
    email: zod_1.z.string().email(),
    password: zod_1.z.string(),
});
exports.authController = {
    async register(req, res) {
        try {
            const data = registerSchema.parse(req.body);
            const existingUser = await user_1.User.findOne({ email: data.email });
            if (existingUser) {
                return res.status(400).json({ error: 'Email already exists' });
            }
            // Simple pseudo-hash for MVP (in production use bcrypt or argon2)
            const passwordHash = Buffer.from(data.password).toString('base64');
            const user = await user_1.User.create({ email: data.email, passwordHash, name: data.name });
            const token = jsonwebtoken_1.default.sign({ userId: user._id, role: user.role }, config_1.config.jwt.secret, { expiresIn: config_1.config.jwt.expiresIn });
            res.status(201).json({ token, user: { id: user._id, name: user.name, email: user.email } });
        }
        catch (err) {
            if (err instanceof zod_1.z.ZodError) {
                return res.status(400).json({ error: 'Validation failed', details: err.errors });
            }
            res.status(500).json({ error: 'Registration failed' });
        }
    },
    async login(req, res) {
        try {
            const data = loginSchema.parse(req.body);
            const passwordHash = Buffer.from(data.password).toString('base64');
            const user = await user_1.User.findOne({ email: data.email, passwordHash });
            if (!user) {
                return res.status(401).json({ error: 'Invalid credentials' });
            }
            const token = jsonwebtoken_1.default.sign({ userId: user._id, role: user.role }, config_1.config.jwt.secret, { expiresIn: config_1.config.jwt.expiresIn });
            res.json({ token, user: { id: user._id, name: user.name, email: user.email } });
        }
        catch (err) {
            if (err instanceof zod_1.z.ZodError) {
                return res.status(400).json({ error: 'Validation failed', details: err.errors });
            }
            res.status(500).json({ error: 'Login failed' });
        }
    },
    async me(req, res) {
        try {
            const user = await user_1.User.findById(req.user.userId).select('-passwordHash');
            if (!user)
                return res.status(404).json({ error: 'User not found' });
            res.json({ user });
        }
        catch (err) {
            res.status(500).json({ error: 'Failed to fetch user' });
        }
    }
};
//# sourceMappingURL=auth.js.map