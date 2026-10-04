import { Router } from 'express';
import { bookmarksController } from '../../controllers/bookmarks';
import { authenticate } from '../../middleware/auth';

export const bookmarksRouter = Router();

bookmarksRouter.use(authenticate);
bookmarksRouter.get('/', bookmarksController.getBookmarks);
bookmarksRouter.post('/:problemId', bookmarksController.addBookmark);
bookmarksRouter.delete('/:problemId', bookmarksController.removeBookmark);
