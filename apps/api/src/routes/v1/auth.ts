import { Router } from 'express';
import { authController } from '../../controllers/auth';
import { authenticate } from '../../middleware/auth';

export const authRouter = Router();

authRouter.post('/register', authController.register);
authRouter.post('/login', authController.login);
authRouter.get('/me', authenticate, authController.me);
