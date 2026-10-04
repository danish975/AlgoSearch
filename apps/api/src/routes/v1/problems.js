"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.problemsRouter = void 0;
const express_1 = require("express");
const problems_1 = require("../../controllers/problems");
exports.problemsRouter = (0, express_1.Router)();
exports.problemsRouter.get('/:id', problems_1.problemsController.getProblem);
exports.problemsRouter.get('/:id/related', problems_1.problemsController.getRelated);
//# sourceMappingURL=problems.js.map