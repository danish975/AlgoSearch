"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.searchController = void 0;
const express_1 = require("express");
const searchEngine_1 = require("../services/searchEngine");
const searchHistory_1 = require("../models/searchHistory");
const redis_1 = require("../services/redis");
const config_1 = require("../config");
exports.searchController = {
    async search(req, res) {
        try {
            const queryParams = req.query;
            const cacheKey = `algosearch:search:${JSON.stringify(queryParams)}`;
            // Check cache
            const cached = await redis_1.redisClient.get(cacheKey);
            if (cached) {
                return res.json(JSON.parse(cached));
            }
            const results = await searchEngine_1.searchEngineService.search(queryParams);
            // Save to cache
            await redis_1.redisClient.set(cacheKey, JSON.stringify(results), { EX: config_1.config.redis.ttlSearch });
            // Save history if user is logged in (optional auth handled in route)
            if (req.user && queryParams.q) {
                await searchHistory_1.SearchHistory.create({ userId: req.user.userId, query: queryParams.q });
            }
            res.json(results);
        }
        catch (err) {
            res.status(502).json({ error: err.message });
        }
    },
    async autocomplete(req, res) {
        try {
            const { q, limit } = req.query;
            const cacheKey = `algosearch:autocomplete:${q}:${limit}`;
            const cached = await redis_1.redisClient.get(cacheKey);
            if (cached) {
                return res.json(JSON.parse(cached));
            }
            const results = await searchEngine_1.searchEngineService.autocomplete(q, parseInt(limit || '8', 10));
            await redis_1.redisClient.set(cacheKey, JSON.stringify(results), { EX: config_1.config.redis.ttlAutocomplete });
            res.json(results);
        }
        catch (err) {
            res.status(502).json({ error: err.message });
        }
    }
};
//# sourceMappingURL=search.js.map