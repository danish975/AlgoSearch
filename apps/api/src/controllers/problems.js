"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.problemsController = void 0;
const express_1 = require("express");
const searchEngine_1 = require("../services/searchEngine");
exports.problemsController = {
    async getProblem(req, res) {
        try {
            const problem = await searchEngine_1.searchEngineService.getProblem(req.params.id);
            if (!problem)
                return res.status(404).json({ error: 'Problem not found' });
            res.json(problem);
        }
        catch (err) {
            res.status(502).json({ error: err.message });
        }
    },
    async getRelated(req, res) {
        try {
            const related = await searchEngine_1.searchEngineService.getRelatedProblems(req.params.id, parseInt(req.query.limit || '10', 10));
            res.json(related);
        }
        catch (err) {
            res.status(502).json({ error: err.message });
        }
    },
    async getAlgorithms(req, res) {
        try {
            const algos = await searchEngine_1.searchEngineService.getAlgorithms();
            res.json(algos);
        }
        catch (err) {
            res.status(502).json({ error: err.message });
        }
    },
    async getTopics(req, res) {
        try {
            const topics = await searchEngine_1.searchEngineService.getTopics();
            res.json(topics);
        }
        catch (err) {
            res.status(502).json({ error: err.message });
        }
    }
};
//# sourceMappingURL=problems.js.map