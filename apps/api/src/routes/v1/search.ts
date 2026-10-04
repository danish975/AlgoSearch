import { Router } from 'express';
import { searchController } from '../../controllers/search';
import { authenticate } from '../../middleware/auth';

export const searchRouter = Router();

// Optional auth for history tracking
searchRouter.get('/', (req, res, next) => {
  if (req.headers.authorization) {
    authenticate(req, res, () => searchController.search(req, res));
  } else {
    searchController.search(req, res);
  }
});

searchRouter.get('/suggestions', searchController.autocomplete);
