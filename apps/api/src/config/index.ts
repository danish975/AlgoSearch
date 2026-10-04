import dotenv from 'dotenv';
import path from 'path';

// Load .env from root
dotenv.config({ path: path.join(__dirname, '../../../../.env') });

export const config = {
  env: process.env.NODE_ENV || 'development',
  port: parseInt(process.env.PORT || '4000', 10),
  searchEngine: {
    url: process.env.SEARCH_ENGINE_URL || 'http://localhost:8080',
  },
  mongo: {
    uri: process.env.MONGODB_URI || 'mongodb://localhost:27017/algosearch',
  },
  redis: {
    url: process.env.REDIS_URL || 'redis://localhost:6379',
    ttlSearch: parseInt(process.env.REDIS_TTL_SEARCH || '300', 10),
    ttlAutocomplete: parseInt(process.env.REDIS_TTL_AUTOCOMPLETE || '60', 10),
  },
  jwt: {
    secret: process.env.JWT_SECRET || 'fallback-secret-do-not-use-in-production',
    expiresIn: process.env.JWT_EXPIRES_IN || '7d',
  },
  rateLimit: {
    windowMs: parseInt(process.env.RATE_LIMIT_WINDOW_MS || '900000', 10),
    max: parseInt(process.env.RATE_LIMIT_MAX_REQUESTS || '100', 10),
  },
  logLevel: process.env.LOG_LEVEL || 'info',
};
