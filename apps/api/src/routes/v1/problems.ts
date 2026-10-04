import { Router } from 'express';
import { problemsController } from '../../controllers/problems';

export const problemsRouter = Router();

problemsRouter.get('/:id', problemsController.getProblem);
problemsRouter.get('/:id/related', problemsController.getRelated);
