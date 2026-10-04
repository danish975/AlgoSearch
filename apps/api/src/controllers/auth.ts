import { Request, Response } from 'express';
import jwt from 'jsonwebtoken';
import { User } from '../models/user';
import { config } from '../config';
import { z } from 'zod';

const registerSchema = z.object({
  email: z.string().email(),
  password: z.string().min(6), // using simple string since we'll hash it, omitting actual hash for simplicity in this MVP
  name: z.string().min(2),
});

const loginSchema = z.object({
  email: z.string().email(),
  password: z.string(),
});

export const authController = {
  async register(req: Request, res: Response) {
    try {
      const data = registerSchema.parse(req.body);
      const existingUser = await User.findOne({ email: data.email });
      if (existingUser) {
        return res.status(400).json({ error: 'Email already exists' });
      }

      // Simple pseudo-hash for MVP (in production use bcrypt or argon2)
      const passwordHash = Buffer.from(data.password).toString('base64');
      const user = await User.create({ email: data.email, passwordHash, name: data.name });

      const token = jwt.sign({ userId: user._id, role: user.role }, config.jwt.secret, { expiresIn: config.jwt.expiresIn as any });
      res.status(201).json({ token, user: { id: user._id, name: user.name, email: user.email } });
    } catch (err: any) {
      if (err instanceof z.ZodError) {
        return res.status(400).json({ error: 'Validation failed', details: err.issues });
      }
      res.status(500).json({ error: 'Registration failed' });
    }
  },

  async login(req: Request, res: Response) {
    try {
      const data = loginSchema.parse(req.body);
      const passwordHash = Buffer.from(data.password).toString('base64');
      const user = await User.findOne({ email: data.email, passwordHash });
      
      if (!user) {
        return res.status(401).json({ error: 'Invalid credentials' });
      }

      const token = jwt.sign({ userId: user._id, role: user.role }, config.jwt.secret, { expiresIn: config.jwt.expiresIn as any });
      res.json({ token, user: { id: user._id, name: user.name, email: user.email } });
    } catch (err: any) {
      if (err instanceof z.ZodError) {
        return res.status(400).json({ error: 'Validation failed', details: err.issues });
      }
      res.status(500).json({ error: 'Login failed' });
    }
  },

  async me(req: any, res: Response) {
    try {
      const user = await User.findById(req.user.userId).select('-passwordHash');
      if (!user) return res.status(404).json({ error: 'User not found' });
      res.json({ user });
    } catch (err) {
      res.status(500).json({ error: 'Failed to fetch user' });
    }
  }
};
