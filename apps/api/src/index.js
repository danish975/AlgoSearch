"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const cors_1 = __importDefault(require("cors"));
const mongoose_1 = __importDefault(require("mongoose"));
const config_1 = require("./config");
const logger_1 = require("./utils/logger");
const errorHandler_1 = require("./middleware/errorHandler");
const v1_1 = require("./routes/v1");
const redis_1 = require("./services/redis");
const app = (0, express_1.default)();
app.use((0, cors_1.default)());
app.use(express_1.default.json());
// Routes
app.use('/api/v1', v1_1.v1Router);
app.get('/health', (req, res) => {
    res.json({ status: 'ok', service: 'algosearch-api' });
});
// Error handling
app.use(errorHandler_1.errorHandler);
async function startServer() {
    try {
        // Connect to MongoDB
        await mongoose_1.default.connect(config_1.config.mongo.uri);
        logger_1.logger.info('Connected to MongoDB');
        // Connect to Redis
        await (0, redis_1.connectRedis)();
        app.listen(config_1.config.port, () => {
            logger_1.logger.info(`API Server running on port ${config_1.config.port}`);
        });
    }
    catch (err) {
        logger_1.logger.error('Failed to start server', err);
        process.exit(1);
    }
}
startServer();
//# sourceMappingURL=index.js.map