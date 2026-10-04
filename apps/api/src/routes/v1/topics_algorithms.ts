import { Router } from 'express';
import { problemsController } from '../../controllers/problems';

export const topicsRouter = Router();
topicsRouter.get('/', problemsController.getTopics);

export const algorithmsRouter = Router();
algorithmsRouter.get('/', problemsController.getAlgorithms);
