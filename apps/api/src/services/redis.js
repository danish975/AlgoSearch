"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.connectRedis = exports.redisClient = void 0;
const redis_1 = require("redis");
const config_1 = require("../config");
const logger_1 = require("../utils/logger");
exports.redisClient = (0, redis_1.createClient)({
    url: config_1.config.redis.url,
});
exports.redisClient.on('error', (err) => logger_1.logger.error('Redis Client Error', err));
const connectRedis = async () => {
    try {
        await exports.redisClient.connect();
        logger_1.logger.info('Connected to Redis');
    }
    catch (err) {
        logger_1.logger.error('Failed to connect to Redis', err);
    }
};
exports.connectRedis = connectRedis;
//# sourceMappingURL=redis.js.map