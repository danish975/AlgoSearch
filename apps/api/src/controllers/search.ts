import { Request, Response } from 'express';
import { searchEngineService } from '../services/searchEngine';
import { SearchHistory } from '../models/searchHistory';
import { redisClient } from '../services/redis';
import { config } from '../config';

export const searchController = {
  async search(req: any, res: Response) {
    try {
      const queryParams = req.query;
      const cacheKey = `algosearch:search:${JSON.stringify(queryParams)}`;

      // Check cache
      const cached = await redisClient.get(cacheKey);
      if (cached) {
        return res.json(JSON.parse(cached));
      }

      const results = await searchEngineService.search(queryParams);

      // Save to cache
      await redisClient.set(cacheKey, JSON.stringify(results), { EX: config.redis.ttlSearch });

      // Save history if user is logged in (optional auth handled in route)
      if (req.user && queryParams.q) {
        await SearchHistory.create({ userId: req.user.userId, query: queryParams.q as string });
      }

      res.json(results);
    } catch (err: any) {
      res.status(502).json({ error: err.message });
    }
  },

  async autocomplete(req: Request, res: Response) {
    try {
      const { q, limit } = req.query;
      const cacheKey = `algosearch:autocomplete:${q}:${limit}`;

      const cached = await redisClient.get(cacheKey);
      if (cached) {
        return res.json(JSON.parse(cached));
      }

      const results = await searchEngineService.autocomplete(q as string, parseInt(limit as string || '8', 10));

      await redisClient.set(cacheKey, JSON.stringify(results), { EX: config.redis.ttlAutocomplete });

      res.json(results);
    } catch (err: any) {
      res.status(502).json({ error: err.message });
    }
  }
};
