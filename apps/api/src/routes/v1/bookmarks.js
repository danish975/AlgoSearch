"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.bookmarksRouter = void 0;
const express_1 = require("express");
const bookmarks_1 = require("../../controllers/bookmarks");
const auth_1 = require("../../middleware/auth");
exports.bookmarksRouter = (0, express_1.Router)();
exports.bookmarksRouter.use(auth_1.authenticate);
exports.bookmarksRouter.get('/', bookmarks_1.bookmarksController.getBookmarks);
exports.bookmarksRouter.post('/:problemId', bookmarks_1.bookmarksController.addBookmark);
exports.bookmarksRouter.delete('/:problemId', bookmarks_1.bookmarksController.removeBookmark);
//# sourceMappingURL=bookmarks.js.map