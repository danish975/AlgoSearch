export declare const config: {
    env: string;
    port: number;
    searchEngine: {
        url: string;
    };
    mongo: {
        uri: string;
    };
    redis: {
        url: string;
        ttlSearch: number;
        ttlAutocomplete: number;
    };
    jwt: {
        secret: string;
        expiresIn: string;
    };
    rateLimit: {
        windowMs: number;
        max: number;
    };
    logLevel: string;
};
//# sourceMappingURL=index.d.ts.map