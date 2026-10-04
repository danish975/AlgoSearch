import { Router } from 'express';
import { authRouter } from './auth';
import { searchRouter } from './search';
import { problemsRouter } from './problems';
import { bookmarksRouter } from './bookmarks';
import { topicsRouter, algorithmsRouter } from './topics_algorithms';

export const v1Router = Router();

v1Router.use('/auth', authRouter);
v1Router.use('/search', searchRouter);
v1Router.use('/problems', problemsRouter);
v1Router.use('/bookmarks', bookmarksRouter);
v1Router.use('/topics', topicsRouter);
v1Router.use('/algorithms', algorithmsRouter);
