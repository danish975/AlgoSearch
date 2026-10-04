"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.v1Router = void 0;
const express_1 = require("express");
const auth_1 = require("./auth");
const search_1 = require("./search");
const problems_1 = require("./problems");
const bookmarks_1 = require("./bookmarks");
const topics_algorithms_1 = require("./topics_algorithms");
exports.v1Router = (0, express_1.Router)();
exports.v1Router.use('/auth', auth_1.authRouter);
exports.v1Router.use('/search', search_1.searchRouter);
exports.v1Router.use('/problems', problems_1.problemsRouter);
exports.v1Router.use('/bookmarks', bookmarks_1.bookmarksRouter);
exports.v1Router.use('/topics', topics_algorithms_1.topicsRouter);
exports.v1Router.use('/algorithms', topics_algorithms_1.algorithmsRouter);
//# sourceMappingURL=index.js.map