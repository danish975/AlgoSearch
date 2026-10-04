import axios from 'axios';

const API_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:4000/api/v1';

export const api = axios.create({
  baseURL: API_URL,
});

api.interceptors.request.use((config) => {
  if (typeof window !== 'undefined') {
    const token = localStorage.getItem('token');
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
  }
  return config;
});

export interface Problem {
  id: string;
  title: string;
  slug: string;
  description: string;
  difficulty: 'Easy' | 'Medium' | 'Hard';
  topics: string[];
  algorithms: string[];
  platform: string;
  externalUrl?: string;
  popularity: number;
  createdAt: string;
}

export interface ScoredResult {
  problem: Problem;
  score: number;
  score_breakdown: Record<string, number>;
}

export interface SearchResponse {
  results: ScoredResult[];
  total_count: number;
  search_duration_ms: number;
  query: string;
}

export interface AutocompleteSuggestion {
  term: string;
  frequency: number;
  category: string;
}
