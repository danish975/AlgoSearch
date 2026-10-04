import { Response } from 'express';
import { Bookmark } from '../models/bookmark';

export const bookmarksController = {
  async getBookmarks(req: any, res: Response) {
    try {
      const bookmarks = await Bookmark.find({ userId: req.user.userId }).sort({ createdAt: -1 });
      res.json(bookmarks);
    } catch (err) {
      res.status(500).json({ error: 'Failed to fetch bookmarks' });
    }
  },

  async addBookmark(req: any, res: Response) {
    try {
      const { problemId } = req.params;
      const { problemTitle, difficulty } = req.body;
      
      const bookmark = await Bookmark.create({
        userId: req.user.userId,
        problemId,
        problemTitle,
        difficulty
      });
      res.status(201).json(bookmark);
    } catch (err: any) {
      if (err.code === 11000) {
        return res.status(400).json({ error: 'Already bookmarked' });
      }
      res.status(500).json({ error: 'Failed to add bookmark' });
    }
  },

  async removeBookmark(req: any, res: Response) {
    try {
      const { problemId } = req.params;
      await Bookmark.findOneAndDelete({ userId: req.user.userId, problemId });
      res.json({ success: true });
    } catch (err) {
      res.status(500).json({ error: 'Failed to remove bookmark' });
    }
  }
};
