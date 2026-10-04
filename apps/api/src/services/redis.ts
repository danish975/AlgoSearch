import { createClient } from 'redis';
import { config } from '../config';
import { logger } from '../utils/logger';

export const redisClient = createClient({
  url: config.redis.url,
});

redisClient.on('error', (err) => logger.error(err, 'Redis Client Error'));

export const connectRedis = async () => {
  try {
    await redisClient.connect();
    logger.info('Connected to Redis');
  } catch (err: any) {
    logger.error(err, 'Failed to connect to Redis');
  }
};
