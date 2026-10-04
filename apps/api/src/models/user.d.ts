import mongoose from 'mongoose';
export declare const User: mongoose.Model<{
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & mongoose.DefaultTimestampProps, {}, {}, {
    id: string;
}, mongoose.Document<unknown, {}, {
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & mongoose.DefaultTimestampProps, {
    id: string;
}, {
    timestamps: true;
}> & Omit<{
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & mongoose.DefaultTimestampProps & {
    _id: mongoose.Types.ObjectId;
} & {
    __v: number;
}, "id"> & mongoose.HydratedDocumentOverrides<{
    id: string;
}>, mongoose.Schema<any, mongoose.Model<any, any, any, any, any, any, any>, {}, {}, {}, {}, {
    timestamps: true;
}, {
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & mongoose.DefaultTimestampProps, mongoose.Document<unknown, {}, {
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & mongoose.DefaultTimestampProps, {
    id: string;
}, Omit<mongoose.DefaultSchemaOptions, "timestamps"> & {
    timestamps: true;
}> & Omit<{
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & mongoose.DefaultTimestampProps & {
    _id: mongoose.Types.ObjectId;
} & {
    __v: number;
}, "id"> & mongoose.HydratedDocumentOverrides<{
    id: string;
}>, unknown, {
    createdAt: NativeDate;
    updatedAt: NativeDate;
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & {
    _id: mongoose.Types.ObjectId;
} & {
    __v: number;
}>, {
    createdAt: NativeDate;
    updatedAt: NativeDate;
    email: string;
    passwordHash: string;
    name: string;
    role: "admin" | "user";
} & {
    _id: mongoose.Types.ObjectId;
} & {
    __v: number;
}>;
//# sourceMappingURL=user.d.ts.map