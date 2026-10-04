"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.bookmarksController = void 0;
const express_1 = require("express");
const bookmark_1 = require("../models/bookmark");
exports.bookmarksController = {
    async getBookmarks(req, res) {
        try {
            const bookmarks = await bookmark_1.Bookmark.find({ userId: req.user.userId }).sort({ createdAt: -1 });
            res.json(bookmarks);
        }
        catch (err) {
            res.status(500).json({ error: 'Failed to fetch bookmarks' });
        }
    },
    async addBookmark(req, res) {
        try {
            const { problemId } = req.params;
            const { problemTitle, difficulty } = req.body;
            const bookmark = await bookmark_1.Bookmark.create({
                userId: req.user.userId,
                problemId,
                problemTitle,
                difficulty
            });
            res.status(201).json(bookmark);
        }
        catch (err) {
            if (err.code === 11000) {
                return res.status(400).json({ error: 'Already bookmarked' });
            }
            res.status(500).json({ error: 'Failed to add bookmark' });
        }
    },
    async removeBookmark(req, res) {
        try {
            const { problemId } = req.params;
            await bookmark_1.Bookmark.findOneAndDelete({ userId: req.user.userId, problemId });
            res.json({ success: true });
        }
        catch (err) {
            res.status(500).json({ error: 'Failed to remove bookmark' });
        }
    }
};
//# sourceMappingURL=bookmarks.js.map