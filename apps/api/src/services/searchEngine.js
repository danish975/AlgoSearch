"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.searchEngineService = void 0;
const axios_1 = __importDefault(require("axios"));
const config_1 = require("../config");
const logger_1 = require("../utils/logger");
const apiClient = axios_1.default.create({
    baseURL: config_1.config.searchEngine.url,
    timeout: 5000,
});
exports.searchEngineService = {
    async search(params) {
        try {
            const response = await apiClient.get('/api/search', { params });
            return response.data;
        }
        catch (err) {
            logger_1.logger.error('Search Engine search error', err.message);
            throw new Error('Search Engine unavailable');
        }
    },
    async autocomplete(q, limit = 8) {
        try {
            const response = await apiClient.get('/api/autocomplete', { params: { q, limit } });
            return response.data;
        }
        catch (err) {
            logger_1.logger.error('Search Engine autocomplete error', err.message);
            throw new Error('Search Engine unavailable');
        }
    },
    async getProblem(id) {
        try {
            const response = await apiClient.get(`/api/problems/${id}`);
            return response.data;
        }
        catch (err) {
            if (err.response && err.response.status === 404)
                return null;
            logger_1.logger.error('Search Engine getProblem error', err.message);
            throw new Error('Search Engine unavailable');
        }
    },
    async getRelatedProblems(id, limit = 10) {
        try {
            const response = await apiClient.get(`/api/problems/${id}/related`, { params: { limit } });
            return response.data;
        }
        catch (err) {
            logger_1.logger.error('Search Engine getRelated error', err.message);
            throw new Error('Search Engine unavailable');
        }
    },
    async getTopics() {
        try {
            const response = await apiClient.get('/api/topics');
            return response.data;
        }
        catch (err) {
            logger_1.logger.error('Search Engine getTopics error', err.message);
            throw new Error('Search Engine unavailable');
        }
    },
    async getAlgorithms() {
        try {
            const response = await apiClient.get('/api/algorithms');
            return response.data;
        }
        catch (err) {
            logger_1.logger.error('Search Engine getAlgorithms error', err.message);
            throw new Error('Search Engine unavailable');
        }
    }
};
//# sourceMappingURL=searchEngine.js.map