import express from 'express';
import cors from 'cors';
import mongoose from 'mongoose';
import { config } from './config';
import { logger } from './utils/logger';
import { errorHandler } from './middleware/errorHandler';
import { v1Router } from './routes/v1';
import { connectRedis } from './services/redis';

const app = express();

app.use(cors());
app.use(express.json());

// Routes
app.use('/api/v1', v1Router);

app.get('/health', (req, res) => {
  res.json({ status: 'ok', service: 'algosearch-api' });
});

// Error handling
app.use(errorHandler);

async function startServer() {
  try {
    // Connect to MongoDB
    await mongoose.connect(config.mongo.uri);
    logger.info('Connected to MongoDB');

    // Connect to Redis
    await connectRedis();

    app.listen(config.port, () => {
      logger.info(`API Server running on port ${config.port}`);
    });
  } catch (err: any) {
    logger.error(err, 'Failed to start server');
    process.exit(1);
  }
}

startServer();
