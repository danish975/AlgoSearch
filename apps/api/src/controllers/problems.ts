import { Request, Response } from 'express';
import { searchEngineService } from '../services/searchEngine';

export const problemsController = {
  async getProblem(req: Request, res: Response) {
    try {
      const problem = await searchEngineService.getProblem(req.params.id as string);
      if (!problem) return res.status(404).json({ error: 'Problem not found' });
      res.json(problem);
    } catch (err: any) {
      res.status(502).json({ error: err.message });
    }
  },

  async getRelated(req: Request, res: Response) {
    try {
      const related = await searchEngineService.getRelatedProblems(req.params.id as string, parseInt(req.query.limit as string || '10', 10));
      res.json(related);
    } catch (err: any) {
      res.status(502).json({ error: err.message });
    }
  },

  async getAlgorithms(req: Request, res: Response) {
    try {
      const algos = await searchEngineService.getAlgorithms();
      res.json(algos);
    } catch (err: any) {
      res.status(502).json({ error: err.message });
    }
  },

  async getTopics(req: Request, res: Response) {
    try {
      const topics = await searchEngineService.getTopics();
      res.json(topics);
    } catch (err: any) {
      res.status(502).json({ error: err.message });
    }
  }
};
