"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.algorithmsRouter = exports.topicsRouter = void 0;
const express_1 = require("express");
const problems_1 = require("../../controllers/problems");
exports.topicsRouter = (0, express_1.Router)();
exports.topicsRouter.get('/', problems_1.problemsController.getTopics);
exports.algorithmsRouter = (0, express_1.Router)();
exports.algorithmsRouter.get('/', problems_1.problemsController.getAlgorithms);
//# sourceMappingURL=topics_algorithms.js.map