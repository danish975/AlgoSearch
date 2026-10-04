import axios from 'axios';
import { config } from '../config';
import { logger } from '../utils/logger';

const apiClient = axios.create({
  baseURL: config.searchEngine.url,
  timeout: 5000,
});

export const searchEngineService = {
  async search(params: any) {
    try {
      const response = await apiClient.get('/api/search', { params });
      return response.data;
    } catch (err: any) {
      logger.error('Search Engine search error', err.message);
      throw new Error('Search Engine unavailable');
    }
  },

  async autocomplete(q: string, limit: number = 8) {
    try {
      const response = await apiClient.get('/api/autocomplete', { params: { q, limit } });
      return response.data;
    } catch (err: any) {
      logger.error('Search Engine autocomplete error', err.message);
      throw new Error('Search Engine unavailable');
    }
  },

  async getProblem(id: string) {
    try {
      const response = await apiClient.get(`/api/problems/${id}`);
      return response.data;
    } catch (err: any) {
      if (err.response && err.response.status === 404) return null;
      logger.error('Search Engine getProblem error', err.message);
      throw new Error('Search Engine unavailable');
    }
  },

  async getRelatedProblems(id: string, limit: number = 10) {
    try {
      const response = await apiClient.get(`/api/problems/${id}/related`, { params: { limit } });
      return response.data;
    } catch (err: any) {
      logger.error('Search Engine getRelated error', err.message);
      throw new Error('Search Engine unavailable');
    }
  },

  async getTopics() {
    try {
      const response = await apiClient.get('/api/topics');
      return response.data;
    } catch (err: any) {
      logger.error('Search Engine getTopics error', err.message);
      throw new Error('Search Engine unavailable');
    }
  },

  async getAlgorithms() {
    try {
      const response = await apiClient.get('/api/algorithms');
      return response.data;
    } catch (err: any) {
      logger.error('Search Engine getAlgorithms error', err.message);
      throw new Error('Search Engine unavailable');
    }
  }
};
